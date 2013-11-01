#include "PlayerCharacter.h"
#include "GameBase.h"
#include "PacketProtocol.h"
#include "CharacterController.h"
#include <math.h>
#include <assert.h>




// ------------------------------------------------------------------------------------------------
PlayerCharacter :: PlayerCharacter(Uint32 setId, GameBase* setGame, CharacterType* setType)
    :	EntityBase(setId, setGame),
        vel(0, 0, 0),
        acc(setType->acc),
        maxSpeed(setType->maxSpeed),
		attackDelay(0),
		targetId(INVALID_ID) {
	type = setType;
    SetHealth(type->maxHealth);
	controller = 0;
	sceneNode = sceneManager->addCubeSceneNode(1.0f, 0, -1, vector3df(0,0,0));
	sceneNode->setMaterialFlag(EMF_LIGHTING, true);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
PlayerCharacter :: ~PlayerCharacter() {

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Spawn(vector3df atPos) {
    Activate();
    SetPos(atPos);
    SetHealth(type->maxHealth);
    attackDelay = 0;
    vel = vector3df(0, 0, 0);
    acc = type->acc;
    maxSpeed = type->maxSpeed;
    targetId = INVALID_ID;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Update(float timeDelta) {
    if(IsActive() && !IsDead()) {
        UpdateVelocity(timeDelta);
        UpdatePosition(timeDelta);
        UpdateFriction(timeDelta);
        UpdateAttack(timeDelta);
		controller->Update();
		sceneNode->setPosition(Pos());
    }
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AutoAttack() {
	if(attackDelay <= 0 && targetId != INVALID_ID) {
		type->autoAttackAction->Activate(this);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: SetTargetId(Uint32 setId) {
	targetId = setId;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AddActionDelay(Uint32 amount) {
	attackDelay += amount;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateVelocity(float timeDelta) {
    // Apply acceleration to velocity based on key states:
    if(controller->MoveHeading().Z < 0) {
        vel.Z -= acc * timeDelta;
    }
    else if(controller->MoveHeading().Z > 0) {
        vel.Z += acc * timeDelta;
    }

    if(controller->MoveHeading().X < 0) {
        vel.X -= acc * timeDelta;
    }
    else if(controller->MoveHeading().X > 0) {
        vel.X += acc * timeDelta;
    }

    // Cap the velocity at the maximum speed:
    if(vel.getLength()  > maxSpeed) {
        vel = vel.normalize();
		vel *= maxSpeed;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdatePosition(float timeDelta) {
	/*
    float charTop = Pos().y - type->height / 2;
    float charBottem = Pos().y + type->height / 2;
    float charLeft = Pos().x - type->width / 2;
    float charRight = Pos().x + type->width / 2;
	vector3df newPos = Pos();

    // Handle velocity to the right:
    if(vel.X > 0) {
        if(!Game()->CheckCollisionWithLevel( vector3df(charRight + vel.X * timeDelta, charTop + 2))
		&& !Game()->CheckCollisionWithLevel( vector3df(charRight + vel.X * timeDelta, charBottem - 2))) {

            newPos.x += vel.X * timeDelta;
        }
        else {
            vel.X = 0;
        }
    }
    // Handle velocity to the left:
    else if(vel.X < 0) {
        if(!Game()->CheckCollisionWithLevel(vector3df(charLeft + vel.X * timeDelta, charTop + 2))
		&& !Game()->CheckCollisionWithLevel(vector3df(charLeft + vel.X * timeDelta, charBottem - 2))) {
            newPos.x += vel.X * timeDelta;
        }
        else {
            vel.X = 0;
        }
    }

    // Handle velocity down:
    if(vel.Z > 0) {
        if(!Game()->CheckCollisionWithLevel(vector3df(charRight - 2, charBottem + vel.Z * timeDelta))
		&& !Game()->CheckCollisionWithLevel(vector3df(charLeft + 2, charBottem + vel.Z * timeDelta))) {
            newPos.y += vel.Z * timeDelta;
        }
        else {
            vel.Z = 0;
        }
    }
    // Handle velocity up
    else if(vel.Z < 0) {
        if(!Game()->CheckCollisionWithLevel(vector3df(charRight - 2, charTop + vel.Z * timeDelta))
		&& !Game()->CheckCollisionWithLevel(vector3df(charLeft + 2, charTop + vel.Z * timeDelta))) {
            newPos.y += vel.Z;
        }
        else {
            vel.Z = 0;
        }
    }
	*/
	vector3df newPos = Pos();
	newPos += vel;
    SetPos(newPos);
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateFriction(float timeDelta) {
    vel.X *= powf(type->friction, timeDelta);
    vel.Z *= powf(type->friction, timeDelta);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateAttack(float timeDelta) {

    if(attackDelay > 0) {
        attackDelay -= timeDelta;
    }
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
vector3df PlayerCharacter :: Heading() const {
    return controller->MoveHeading();
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
bool PlayerCharacter :: IsSolid(vector3df atPos) const {
	return 	atPos.getDistanceFrom(Pos()) < type->width;
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AddHealth(Uint32 amount) {
    Uint32 setHealth = Health() + amount;

    if(setHealth > type->maxHealth) {
        setHealth = type->maxHealth;
    }

    SetHealth(setHealth);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: SetRespawnMe(bool b) {
	respawnMe = b;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: SetRespawnTime(Uint32 time) {
	respawnTime = time;
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
bool PlayerCharacter :: GetRespawnMe() const {
	return respawnMe;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: GetRespawnTime() const {
	return respawnTime;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool PlayerCharacter :: IsReadyToAttack() const {
	return attackDelay <= 0;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: WriteToPacket(Uint32 dataWritePos, Uint8 data[]) {
	Uint32 sendId = Id();
	vector3df sendPos = Pos();
	Uint32 sendHealth = Health();

    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_ID], &sendId,4 );
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_POSX], &sendPos.X, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_POSY], &sendPos.Y, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_VELX], &vel.X, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_VELY], &vel.Z, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_HEALTH], &sendHealth, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_CONTROL_STATE], controller, sizeof(CharacterController));

    return PACKET_WRITE_PLAYER_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[]) {
	vector3df readPos;
    CharacterController readController(this);
    Uint32 readHealth;

    memcpy(&readPos.X, &data[dataReadPos + PACKET_WRITE_PLAYER_POSX], 4);
    memcpy(&readPos.Y, &data[dataReadPos + PACKET_WRITE_PLAYER_POSY], 4);
    memcpy(&vel.X, &data[dataReadPos + PACKET_WRITE_PLAYER_VELX], 4);
    memcpy(&vel.Z, &data[dataReadPos + PACKET_WRITE_PLAYER_VELY], 4);
    memcpy(&readHealth, &data[dataReadPos + PACKET_WRITE_PLAYER_HEALTH], 4);
    memcpy(&readController, &data[dataReadPos + PACKET_WRITE_PLAYER_CONTROL_STATE], sizeof(CharacterController));

	SetPos(readPos);
	SetControllerState(&readController);
	SetHealth(readHealth);
    Activate();
    return PACKET_WRITE_PLAYER_LENGTH;

    /*
    if(playerID == myChannel) {
    	// Apply the remaining commands to the character:
    	while(commands.size() > 0) {
    		myPc->SetCommands(commands.front()->data);
    		myPc->UpdateVelocity();
    		myPc->UpdatePosition();

    		delete commands.front();
    		commands.pop_front();
    	}
    }*/
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
CharacterController* PlayerCharacter :: GetController() const {
	return controller;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
 Uint32 PlayerCharacter :: TargetId() const {
 	return targetId;
 } // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: SetController(CharacterController* newController) {
	if(controller != 0) {
		delete controller;
	}

	controller = newController;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: SetControllerState(CharacterController* newState) {
	controller->SetState(newState);
} // ----------------------------------------------------------------------------------------------

