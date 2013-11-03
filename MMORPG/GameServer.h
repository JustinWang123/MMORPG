#ifndef SERVER_H
#define SERVER_H

#include "GameBase.h"
class CharacterController;

using namespace std;

const										Uint32 PLAYER_TIME_OUT = 10000; // 10 seconds

class GameServer : public GameBase {
public:
											GameServer();
											~GameServer();

    virtual void							Update();
    virtual void							Draw();

private:
	// Network support methods:
    void									SendDataAsPacket(int to, char data[], int length);
    void									AckSequenceNum(Uint32 playerId, Uint32 num);
    void									StorePacket(Uint32 playerId, char data[], Uint32 length);
    ListablePacket*							GetStoredPacket(Uint32 playerId, Uint32 sequenceNum);
	void 									ReceiveNetworkData();
    void									DisconnectPlayer(Uint32 playerID);

	//	Handle incoming packets:
    void									HandleConnectionRequest(UDPpacket* packet);
    void									HandleDisconnectNotification(UDPpacket* packet);
    void									HandleMsg(UDPpacket* packet);
    void									HandleControlState(UDPpacket* packet);
    void									HandlePlayerUpdate(UDPpacket* packet);

	// Send packets to clients:
    void 									SendNew(int about, int to);
    void									SendConnectionAccepted(int to);
    void									SendDisconnectNotification(int to, int about);
    void									SendTextMessage(int to, int from, std::string msg);
    void									SendWorldUpdateToAll();
    void									SendWorldUpdate(int to);
    void									SendSpawn(int to, int about);
    void									SendKillMessage(int to, int killerPlayerID, int deadPlayerID);
    void									SendKillMessageToAll(int killerPlayerID, int deadPlayerID);
    void									SendLevelData(int to);

    Player									players[MAX_PLAYERS];
	std::map<Uint32, CharacterController*>	characterControllers;
    Uint32									numOfPlayers;
};

#endif
