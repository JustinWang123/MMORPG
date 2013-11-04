#include "GameClient.h"
#include <math.h>
#include <assert.h>
#include <fstream>
#include "PacketProtocol.h"
#include "PlayerCharacterView.h"
#include "CharacterControllerClient.h"
#include "NullCharacterController.h"
using namespace std;




// ------------------------------------------------------------------------------------------------
GameClient :: GameClient() {
    // Open a UDPsocket on the first unused socket:
    for (int i = 0; i < 10; i++) {
        socket = SDLNet_UDP_Open(NETWORK_PORT + i);

        if(socket != NULL) {
            port = NETWORK_PORT + i;
            break;
        }
    }

    std::ifstream clientFile ("Data/ClientInit.txt");
    assert(clientFile.is_open());
    std::string str;
    std::string ipAddressStr;
    clientFile >> str;
    assert(str == "ServerIP:" && "Somethings wrong with data/ClientInit.txt");
    clientFile >> ipAddressStr;
    clientFile >> str;
    assert(str == "ClientName:" && "Somethings wrong with data/ClientInit.txt");
    clientFile >> clientName;
    clientFile.close();

    if(SDLNet_ResolveHost(&serverIP, ipAddressStr.c_str(), NETWORK_PORT)) {
        AddMessagetoChat("Failed to resolve host");
    }

    // Bind the server to channel 0:
    if(SDLNet_UDP_Bind(socket, SERVER, &serverIP)) {
        AddMessagetoChat("Failed to bind socket");
    }

    
    // No players are active at first
    for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
        players[i].state = PLAYER_STATE_DISCONECTED;
    }

    isTypingChatMessage = false;
    myChannel = INVALID_CHANNEL;
    exit = false;
    myPc = 0;
	myPcController = new NullCharacterController();
    localSequenceNum = 0;
    remoteSequenceNum = 0;
    remoteSequenceNumBitField = -1;
    timeToSendNextControlState = device->getTimer()->getTime();
    timeOfLastGameServerMessage = device->getTimer()->getTime();
    nextPacketTime = device->getTimer()->getTime();
} // ------------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
GameClient :: ~GameClient() {
    // Notifiy server that we're disconnecting:
    if(isConnectedToGameServer) {
        SendDisconnectNotification();
    }

    if ( socket != NULL ) {
        SDLNet_UDP_Close(socket);
        socket = NULL;
    }
    if ( packets != NULL ) {
        SDLNet_FreePacketV(packets);
        packets = NULL;
    }

	delete myPcController;
	delete view;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: Update() {
    // Receive all network data this game loop:
    ReceiveNetworkData();

    // Update everything:
    HandleMyPcControlState();
    GameBase::Update();
    UpdateGameServerConnection();
	myPcController->Update();

    // Send network data:
    if(isConnectedToGameServer && device->getTimer()->getTime() > nextPacketTime) {
        SendUpdateGameServer();
        nextPacketTime = device->getTimer()->getTime() + PACKET_SEND_DELAY;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: UpdateGameServerConnection() {
    // Keep attempting to connect to server:
    if(!isConnectedToGameServer && device->getTimer()->getTime() > nextPacketTime) {
        nextPacketTime = device->getTimer()->getTime() + RECONNECT_TO_SERVER_DELAY;
        timeOfLastGameServerMessage = device->getTimer()->getTime();
        SendConnectionRequest(clientName);
    }

    // Determine if we have lost connection to server
    if(isConnectedToGameServer && device->getTimer()->getTime() - timeOfLastGameServerMessage > SERVER_INACTIVITY_TIME) {
        AddMessagetoChat("GameServer not responding...");
        isConnectedToGameServer = false;
    }
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void GameClient :: HandleMyPcControlState() {
    if(myPc != 0) {
        // If enough time has passed then update the server with our controller's state.
		if(myPcController->HasChanged() && device->getTimer()->getTime() > timeToSendNextControlState) {
            timeToSendNextControlState = device->getTimer()->getTime() + MIN_SEND_CONTROL_STATE_TIME;
            SendControllerState();
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: Draw() {
	GameBase::Draw();
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: SendDataAsPacket(char data[], int length) {
    timeOfLastMessageToGameServer = GetTime();

    // Insert the current local sequence num:
    memcpy(&data[PACKET_SEQUENCE_NUM], &localSequenceNum, 4);

    // Copy the data into a packet:
    memcpy(packets[0]->data, data, length);
    packets[0]->len = length;

    SDLNet_UDP_Send(socket, SERVER, packets[0]);

    StorePacket(data, 100);

    localSequenceNum++;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 GameClient :: GetLostPacketSequenceNum(Uint32 ackNum, Uint32 ackBitfield) {
    if( (Uint32)pow((double)2, 31) & ~ackBitfield ) {
        return 1;
    }

    return 0;

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: AckSequenceNum(Uint32 newNum) {
    // If num > remoteSequenceNum
    if(newNum > remoteSequenceNum) {

        // shift bitfield left num - remoteSequenceNum
        remoteSequenceNumBitField = remoteSequenceNumBitField << (newNum - remoteSequenceNum);

        // toggle bit coresponding to previous remoteSequenceNum on.
        remoteSequenceNumBitField = remoteSequenceNumBitField | (unsigned)pow((double)2, (double)newNum - remoteSequenceNum - 1);

        remoteSequenceNum = newNum;
    }
    else if(remoteSequenceNum - newNum <= 32) {
        remoteSequenceNumBitField = remoteSequenceNumBitField | (unsigned)pow((double)2, (double)remoteSequenceNum - newNum - 1);
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: StorePacket(char data[], Uint32 length) {
    assert(length <= LISTABLE_PACKET_DATA_LENGTH);

    // Push new packet onto queue:
    ListablePacket* packet = new ListablePacket();
    memcpy(&packet->data, data, length);
    packet->length = length;
    sentPacketList.push_front(packet);

    // Pop packets if queue is full:
    Uint32 listSize = sentPacketList.size();
    if(listSize > 50) {
        ListablePacket* popPacket;
        popPacket = sentPacketList.back();
        delete popPacket;
        sentPacketList.pop_back();
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
ListablePacket* GameClient :: GetStoredPacket(Uint32 sequenceNum) {
    for(std::deque<ListablePacket*>::iterator it = sentPacketList.begin(); it != sentPacketList.end(); it++) {
        Uint32 tempSequenceNum;
        memcpy(&tempSequenceNum, &((*it)->data[PACKET_SEQUENCE_NUM]), 4);

        if(tempSequenceNum == sequenceNum) {
            return *it;
        }
    }

    return 0;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: ReceiveNetworkData() {
    // Receive n packets of data on our UDP socket:
    int n = SDLNet_UDP_RecvV(socket, packets);

    // For each packet of data we need to look at the first word and determine what to do with it:
    while ( n-- > 0 ) {
        timeOfLastGameServerMessage = device->getTimer()->getTime();

        // Acknowledge packet sequence num:
        Uint32 packetSequenceNum;
        memcpy(&packetSequenceNum, &packets[n]->data[PACKET_SEQUENCE_NUM], 4);
        AckSequenceNum(packetSequenceNum);

        switch (packets[n]->data[0]) {
        case PACKET_MSG:
            HandleTextMessage(packets[n]);
            break;

        case PACKET_UPDATE_WORLD:
            GameBase::HandleUpdateWorld(packets[n]);
            break;

        case PACKET_ADD:
            HandleAddNewGameClient(packets[n]);
            break;

        case PACKET_DEL:
            HandleRemoveGameClient(packets[n]);
            break;

        case PACKET_ACCEPT_CONNECTION:
            HandleConnectionAccepted(packets[n]);
            break;

        case PACKET_SPAWN:
            HandleSpawn(packets[n]);
            break;

        case PACKET_KILL_NOTIFICATION:
            HandleKillNotification(packets[n]);
            break;
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: SendUpdateGameServer() {
    char data[PACKET_PLAYER_UPDATE_SERVER_LENGTH];
    memcpy(&data[0], &PACKET_PLAYER_UPDATE_SERVER, 1);
    memcpy(&data[PACKET_PLAYER_UPDATE_SERVER_SEQUENCE_NUM], &localSequenceNum, 4);
    memcpy(&data[PACKET_PLAYER_UPDATE_SERVER_ACK_NUM], &remoteSequenceNum, 4);
    memcpy(&data[PACKET_PLAYER_UPDATE_SERVER_ACK_BITFIELD], &remoteSequenceNumBitField, 4);

    SendDataAsPacket(data, PACKET_PLAYER_UPDATE_SERVER_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: SendConnectionRequest(std::string name) {
    int nameLength = name.length();

    char data[PACKET_REQUEST_CONNECTION_LENGTH + 100]; // FIXTHIS
    memcpy(&data[0], &PACKET_REQUEST_CONNECTION, 1);
    memcpy(&data[PACKET_REQUEST_CONNECTION_SEQUENCE_NUM], &localSequenceNum, 4);
    memcpy(&data[PACKET_REQUEST_CONNECTION_NLEN], &nameLength, 4);
    memcpy(&data[PACKET_REQUEST_CONNECTION_NAME], name.c_str(), nameLength);

    SendDataAsPacket(data, PACKET_REQUEST_CONNECTION_LENGTH + nameLength);

    AddMessagetoChat("Requesting GameServer Connection...");
} // ------------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: SendDisconnectNotification() {
    char data[PACKET_DISCONNECT_LENGTH];
    memcpy(&data[0], &PACKET_DISCONNECT, 1);
    memcpy(&data[PACKET_DISCONNECT_SEQUENCE_NUM], &localSequenceNum, 4);

    SendDataAsPacket(data, PACKET_DISCONNECT_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: SendTextMessage(std::string msg) {
    int msgLength = msg.length();

    char data[PACKET_PACKETSIZE];
    memcpy(&data[0], &PACKET_MSG, 1);
    memcpy(&data[PACKET_MSG_SEQUENCE_NUM], &localSequenceNum, 4);
    memcpy(&data[PACKET_MSG_NLEN], &msgLength, 4);
    memcpy(&data[PACKET_MSG_TEXT], msg.c_str(), msgLength);

    SendDataAsPacket(data,  PACKET_MSG_LENGTH + msgLength);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
// send myPc's control state to server
void GameClient :: SendControllerState() {
    char data[PACKET_PLAYER_CONTROL_STATE_LENGTH];

    memcpy(&data[0], &PACKET_PLAYER_CONTROL_STATE, 1);
    memcpy(&data[PACKET_PLAYER_CONTROL_STATE_SEQUENCE_NUM], &localSequenceNum, 4);
    memcpy(&data[PACKET_PLAYER_CONTROL_STATE_STATE], myPcController, sizeof(CharacterController));

    SendDataAsPacket(data, PACKET_PLAYER_CONTROL_STATE_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: HandleTextMessage(UDPpacket* packet) {
    Uint32 playerID;
    Uint32 msgLength;

    memcpy(&playerID, &packet->data[PACKET_MSG_PLAYER_ID], 4);
    memcpy(&msgLength, &packet->data[PACKET_MSG_NLEN], 4);

    // Cram the entire message into a string called msg:
    char cmsg[PACKET_PACKETSIZE];
    memcpy(cmsg, &packet->data[PACKET_MSG_TEXT], msgLength);
    std::string msg(cmsg, 0, msgLength);

    if(playerID == MAX_PLAYERS) {
        AddMessagetoChat("GameServer: " + msg);
    }
    else {
        AddMessagetoChat(players[playerID].name + " " + msg);
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: HandleAddNewGameClient(UDPpacket* packet) {
    Uint8 channel = packet->data[PACKET_ADD_SLOT];

    // Grab the new GameClients name an associate with his channel:
    char nameC[100];
    memcpy(nameC, &packet->data[PACKET_ADD_NAME], packet->data[PACKET_ADD_NLEN]);
    players[channel].name.assign(nameC, packet->data[PACKET_ADD_NLEN]);

    // Display a text message:
    AddMessagetoChat(players[channel].name + " has connected on channel: " + ToString(channel));

    // Set the players state to active so that we can begin updating:
    players[channel].state = PLAYER_STATE_ACTIVE;

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: HandleRemoveGameClient(UDPpacket* packet) {
    Uint8 channel = packet->data[PACKET_DEL_SLOT];

    // Display a text message:
    AddMessagetoChat(players[channel].name + " has disconnected from channel: " + ToString(channel));

    players[channel].state = PLAYER_STATE_DISCONECTED;

    if(channel == myChannel) {
        isConnectedToGameServer = false;
        exit = true;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: HandleConnectionAccepted(UDPpacket* packet) {
    // Get channel:
    memcpy(&myChannel, &packet->data[PACKET_ACCEPT_CONNECTION_CHANNEL], 4);

    // Get name:
    players[myChannel].name = clientName;

	// Create pc, controller, view:
    myPc =				GetPlayerCharacter(myChannel);
	myPcController =	new CharacterControllerClient(myPc);
	view =				new PlayerCharacterView(this, myPc);

    // Display a text message:
    AddMessagetoChat("You have established a conection with the server...");
    isConnectedToGameServer = true;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: HandleSpawn(UDPpacket* packet) {
    Uint32 channel;
    Uint32 posX;
    Uint32 posY;

    memcpy(&channel, &packet->data[PACKET_SPAWN_CHANNEL], 4);
    memcpy(&posX, &packet->data[PACKET_SPAWN_POS_X], 4);
    memcpy(&posY, &packet->data[PACKET_SPAWN_POS_Y], 4);


    //players[channel].pc->Spawn(posX, posY); //FIXTHIS i think this method exists so we can add sparklys here
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameClient :: HandleKillNotification(UDPpacket* packet) {
    Uint32 killerID;
    Uint32 killedID;

    memcpy(&killerID, &packet->data[PACKET_KILL_NOTIFICATION_KILLER_ID], 4);
    memcpy(&killedID, &packet->data[PACKET_KILL_NOTIFICATION_KILLED_ID], 4);

    AddMessagetoChat(players[killedID].name + " was killed by " + players[killerID].name);
} // ----------------------------------------------------------------------------------------------