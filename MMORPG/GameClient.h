#ifndef GameClient_H
#define GameClient_H

#include "GameBase.h"
#include <deque>
#include <vector>
class PlayerCharacter;

const Uint32 	SERVER = 0;
const Uint32 	SERVER_INACTIVITY_TIME = 5000; // 5 seconds with no messages means inactive
const Uint32	RECONNECT_TO_SERVER_DELAY = 5000; // 5 seconds
const Uint32	MIN_SEND_CONTROL_STATE_TIME = 30;

class GameClient : public GameBase {
public:
    GameClient();
    ~GameClient();

    virtual void			Update();
    virtual void			Draw();
	
private:
    // Network methods:
    void					SendDataAsPacket(char data[], int length);
    Uint32          		GetLostPacketSequenceNum(Uint32 ackNum, Uint32 ackBitfield);
    void					AckSequenceNum(Uint32 num);
    void					StorePacket(char data[], Uint32 length);
    ListablePacket*			GetStoredPacket(Uint32 sequenceNum);
	void					HandleMyPcControlState();
	void					UpdateGameServerConnection();
    void 					ReceiveNetworkData();

    // Handling of event packets:
    void					HandleTextMessage(UDPpacket* packet);
    void					HandleAddNewGameClient(UDPpacket* packet);
    void					HandleRemoveGameClient(UDPpacket* packet);
    void					HandleConnectionAccepted(UDPpacket* packet);
    void					HandleSpawn(UDPpacket* packet);
    void					HandleKillNotification(UDPpacket* packet);
    void					HandleUpdateWorld(UDPpacket* packet);

    // Sending event packets:
    void					SendUpdateGameServer();
    void					SendTextMessage(std::string msg);
    void 					SendConnectionRequest(std::string name);
    void					SendDisconnectNotification();
    void					SendControllerState();

    // game data members:
    PlayerCharacter*		myPc;
    Uint32					myChannel;
    std::string 			clientName;
    
    // Network stuff:
    Uint32 					localSequenceNum;
    Uint32					remoteSequenceNum;
    Uint32					remoteSequenceNumBitField;
    std::deque<ListablePacket*> sentPacketList;
	Uint32					serverPingTime;
    Uint32					timeOfLastGameServerMessage;
    Uint32					timeOfLastMessageToGameServer;
    Uint32					timeToSendNextControlState;
    bool					isConnectedToGameServer;
    Uint32					timeOfLastHeadingMessage;
};
#endif
