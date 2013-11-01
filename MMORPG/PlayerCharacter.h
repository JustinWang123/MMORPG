#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H

#include "Irr_Wrapper.hpp"
#include "EntityBase.h"
class CharacterController;
class GameBase;
class CharacterAction;

const Uint32				AIMMER_DISTANCE = 32;
const float					CHARACTER_RADIUS = 16.0f;

struct CharacterType {
	Uint32					attackDelay;
	Uint32					maxHealth;
	float					acc;
	float					maxSpeed;
	float					friction;
	Uint32					width;
	Uint32					height;
    CharacterAction*		autoAttackAction;
};

struct QueuedAction {
	CharacterAction* action;
	bool repeat;
};

class PlayerCharacter : public EntityBase {
public:
							PlayerCharacter(Uint32 setId, GameBase* setGame, CharacterType* setType);
    virtual					~PlayerCharacter();
    void					Spawn(vector3df atPos);

    virtual void 			Update(float timeDelta);

    void					AutoAttack();

    Uint32                  WriteToPacket(Uint32 dataWritePos, Uint8 data[]);
    Uint32					ReadFromPacket(Uint32 dataReadPos, Uint8 data[]);

    // Modifiers:
    void					AddHealth(Uint32 amount);
    void					SetRespawnMe(bool b);
    void					SetRespawnTime(Uint32 time);
	void					SetController(CharacterController* newController);
	void					SetControllerState(CharacterController* newState);
	void					SetTargetId(Uint32 setId);
	void					AddActionDelay(Uint32 amount);

    // Accessors:
    virtual bool			IsSolid(vector3df atPos) const;
    bool					GetRespawnMe() const;
    Uint32					GetRespawnTime() const;
    CharacterController*	GetController() const;
    Uint32					TargetId() const;

protected:
    vector3df				Heading() const;
    bool					IsReadyToAttack() const;

    // Update subroutines:
    void 					UpdateVelocity(float timeDelta);
    void					UpdatePosition(float timeDelta);
    void					UpdateFriction(float timeDelta);
    void					UpdateAttack(float timeDelta);
    void					SelectTarget(vector3df camPos);

	CharacterType*			type;
	CharacterController*	controller;
	IMeshSceneNode*			sceneNode;

    // Movement data members:
    vector3df				vel;
    float   				acc;
    float					maxSpeed;


    // Game data members:
    float 					attackDelay;
    //std::queue<QueuedAction> actionQueue;
    bool					isActive;
    Uint32					respawnTime;
    bool					respawnMe;
    Uint32                  targetId;

    // The game that the character belongs to:
    GameBase* 				game;
};
#endif