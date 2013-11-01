#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H

#include "ProjectileBase.h"
#include "Irr_Wrapper.hpp"
class CharacterController;
class GameBase;
class CharacterAction;

typedef Uint32 				Tile;
typedef float 				Pos;

const Uint32				AIMMER_DISTANCE = 32;
const float					CHARACTER_RADIUS = 16.0f;
const Uint32				NUM_OF_STARTING_BULLETS = 100;
const Uint32				CHARACTER_ROTATE_SPEED = 10;
const Uint32				CHARACTER_MAKE_SMOKE_TIME = 5;

enum Facing {
    FACING_RIGHT = 0,
    FACING_LEFT
};

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
    void					Spawn(Vector2df atPos);

    virtual void 			Update(float timeDelta);
    void 					Draw(Vector2df camPos) const;
    void					AutoAttack();

    Uint32                  WriteToPacket(Uint32 dataWritePos, Uint8 data[]);
    Uint32					ReadFromPacket(Uint32 dataReadPos, Uint8 data[]);

    // Modifiers:
    void					AddHealth(Uint32 amount);
    void					AddBullets(Uint32 amount);
    void					AddMissiles(Uint32 amount);
    void					AddMines(Uint32 amount);
    void					SetRespawnMe(bool b);
    void					SetRespawnTime(Uint32 time);
    void					AddPoint();
	void					SetController(CharacterController* newController);
	void					SetControllerState(CharacterController* newState);
	void					SetTargetId(Uint32 setId);
	void					AddActionDelay(Uint32 amount);

    // Accessors:
    Uint32					GetNumOfBullets() const;
    Uint32					GetNumOfMissiles() const;
    Uint32					GetNumOfMines() const;
    Uint32					GetScore() const;
    virtual bool			IsSolid(Vector2df atPos) const;
    bool					GetRespawnMe() const;
    Uint32					GetRespawnTime() const;
    CharacterController*	GetController() const;
    Uint32					TargetId() const;

protected:
    Vector2df				Heading() const;
    bool					IsReadyToAttack() const;

    // Update subroutines:
    void 					UpdateVelocity(float timeDelta);
    void					UpdatePosition(float timeDelta);
    void					UpdateFriction(float timeDelta);
    void					UpdateAttack(float timeDelta);
    void					SelectTarget(Vector2df camPos);

	CharacterType*			type;
	CharacterController*	controller;
	IMeshSceneNode*			sceneNode;

    // Movement data members:
    Vector2df				vel;
    float   				acc;
    float					maxSpeed;

    // Movement state:
    bool					collisionAbove;
    bool					collisionBelow;
    bool					collisionLeft;
    bool					collisionRight;

    // Game data members:
    Uint32					numOfBullets;
    Uint32					numOfMissiles;
    Uint32					numOfMines;
    Uint32					score;
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