#ifndef GameClient_H
#define GameClient_H

#include "GameBase.h"
#include <deque>
#include <vector>
#include "Button.h"
class PlayerCharacter;

const Uint32 	GAME_EXIT_BUTTON_POS_X = 666;
const Uint32 	GAME_EXIT_BUTTON_POS_Y = 570;
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
    Vector2df 				CamPos() const;
    Vector2df				MouseWorldPos() const;

	void					RegisterButton(Button* button);
	void					UnregisterButton(Button* button);

private:
    void					UpdateGameServerConnection();
    void					UpdateCamera();

    // Character Control State:
    void					HandleMyPcControlState();

    // Drawing methods:
    void					DrawHUD();
    void					DrawScoreBoard();

    // Network methods:
    void					SendDataAsPacket(char data[], int length);
    Uint32          		GetLostPacketSequenceNum(Uint32 ackNum, Uint32 ackBitfield);
    void					AckSequenceNum(Uint32 num);
    void					StorePacket(char data[], Uint32 length);
    ListablePacket*			GetStoredPacket(Uint32 sequenceNum);

    // High level network receivers:
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

    // data members:
    PlayerCharacter*		myPc;
    Uint32					myChannel;
    std::string 			clientName;
    bool					isMouseOverExitButton;
    Uint32					serverPingTime;
    Uint32					timeOfLastGameServerMessage;
    Uint32					timeOfLastMessageToGameServer;
    Uint32					timeToSendNextControlState;
    bool					isConnectedToGameServer;
    Uint32					timeOfLastHeadingMessage;

    // Interface:
    Button					exitButton;
    SDL_Surface*			inGameMenu;
	SDL_Surface*            surfaceTargetRing;
	std::vector<Button*>	buttons;


    // Network stuff:
    Uint32 					localSequenceNum;
    Uint32					remoteSequenceNum;
    Uint32					remoteSequenceNumBitField;
    std::deque<ListablePacket*> sentPacketList;
    Vector2df				camPos;


};
#endif
