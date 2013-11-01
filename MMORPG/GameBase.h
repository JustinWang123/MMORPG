#ifndef GAME_SIM_H
#define GAME_SIM_H

#include "SDLWrapper.h"
#include "PlayerCharacter.h"
#include "Map.h"
#include "Player.h"
#include "ProjectileBase.h"
#include "Event.h"
#include "CharacterAction.h"
#include "CharacterActionEffect.h"
#include "CharacterActionCondition.h"
#include <vector>
#include <map>

const Uint32 						MAX_PLAYERS = 10;
const Uint32						MAX_NPCS = 10;
const Uint32						MAX_PROJECTILES = 100;
const Uint32   						RESPAWN_DELAY = 5000;

const Uint32 						GAME_SCREEN_X = 4;
const Uint32 						GAME_SCREEN_Y = 4;
const Uint32 						GAME_SCREEN_WIDTH = 576;
const Uint32 						GAME_SCREEN_HEIGHT = 592;
const Uint32 						INVALID_CHANNEL = MAX_PLAYERS;
const Uint32						INVALID_ID = -1;

const Uint32 						CHAT_LOG_NUM_LINES_VISIBLE = 20;
const Uint32 						CHAT_LOG_LINE_LENGTH = 30;
const Uint32 						CHAT_LOG_POS_X = 590;
const Uint32 						CHAT_LOG_POS_Y = 156;
const Uint32 						CHAT_MESSAGE_POS_X = 600;
const Uint32 						CHAT_MESSAGE_POS_Y = 156;

class GameBase {
public:
    GameBase();
    virtual							~GameBase();

    virtual void					Update();
    virtual void					Draw(Vector2df camPos);

    void							LoadMap(std::string fileName);

    void 							HandleUpdateWorld(UDPpacket* packet);
    Uint32 							WriteGameStateToPacket(Uint8 data[]);

    void 							HandleLevelData(UDPpacket* packet);
    Uint32 							WriteLevelDataToPacket(Uint8 data[]);

    // TEMPORARY:
    void							MakeCharacterControllable(Uint32 id);
    PlayerCharacter*				GetPlayerCharacter(Uint32 playerId) {return playerCharacters[playerId];}
    Uint32							GetPlayerCharacterScore(Uint32 playerId){return playerCharacters[playerId]->GetScore();}

    // Events:
    void							DamageCharsInCircle(Uint32 attackerID, Uint32 damage, Vector2df centerPos, float radius);
    void							DamageCharsAtPos(Uint32 attackerID, Uint32 damage, Vector2df atPos);
    PlayerCharacter*                GetCharacter(Uint32 id);
	Uint32							GetCharacterIdAtPos(Vector2df pos);

    // Creates:
    void							SpawnPlayerCharacter(Uint32 playerID);
    void							CreateProjectile(ProjectileType type, int ownerPlayerID, Vector2df setPos, Vector2df setHeading);
    void							CreateExplosion(int ownerPlayerID, Vector2df setPos);

    // Accessors:
    bool							CheckCollisionWithLevel(Vector2df atPos) const;
    bool							CheckCollisionWithChars(Vector2df atPos) const;
    bool							IsPosSolid(Vector2df atPos) const;
    bool							IsPosGravityWell(Vector2df atPos) const;
    Vector2df						GetPosGravity(Vector2df atPos) const;
    Vector2df						GetClosestCharPos(Vector2df atPos) const;
	bool							GetExit() const;

protected:
	// Network subroutines:
	Uint32							ReadProjectilePacket(Uint32 dataReadPos, Uint8 data[]);

    // Update subroutines:
    void							UpdateAllEntities(float timeDelta);
    void							UpdateRespawn(float timeDelta);

    // Draw subroutines:
    void							DrawAllEntities(Vector2df camPos);
    void							DrawMap(Vector2df camPos);

	// Entity subroutines:
	void							SetProjectileType(Uint32 id, ProjectileType setType);

	// Text IO methods:
    void							InputChatMessage();
    void							InputCharToChatMessage();
    void							AddMessagetoChat(std::string msg);
    void							DrawChatLog();
    void							DrawChatMessage();

    // Game data members:
    std::map<Uint32, EntityBase*>		entities;
    std::map<Uint32, PlayerCharacter*>	playerCharacters;
    std::map<Uint32, PlayerCharacter*>	nonPlayerCharacters;
    std::map<Uint32, PlayerCharacter*>	characters;
    std::map<Uint32, ProjectileBase*> 	projectiles;
    Map*							gameMap;
	Event*							events[NUM_OF_EVENT_TYPES];
	CharacterAction*				characterActions[NUM_OF_CHARACTER_ACTION_TYPES];
	CharacterActionEffect*			characterActionEffects[NUM_OF_CHARACTER_ACTION_EFFECT_TYPES];
	CharacterActionCondition*		characterActionConditions[NUM_OF_CHARACTER_ACTION_CONDITION_TYPES];
	Player							players[MAX_PLAYERS];
	Uint32							oldTime; // for timing game loop
	bool							exit;
	vector<CharacterType*>			characterTypes;

    // Text IO data members:
    bool 							isTypingChatMessage;
    std::string						chatMessage;
    Uint32							chatMessageCursorPos;
    vector<std::string>				chatLog; // The chatLog stores and displays messages received by the GameClient:
	IGUIWindow*						window;
	IGUIListBox*					chatWindow;

    // Network data members:
    Uint32							port;
    IPaddress						serverIP;
    UDPsocket 						socket;
    UDPpacket**						packets;
    Uint32							nextPacketTime;

    // Surface data members:
    SDL_Surface*					bulletSurface;
    SDL_Surface*  					missileSurface;
    SDL_Surface*					mineSurface;
    SDL_Surface*					smokeSurface;
    SDL_Surface* 					explosionSurface;
};
#endif
