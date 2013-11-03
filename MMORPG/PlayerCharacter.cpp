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
		lookHeading(1, 0, 0),
		attackDelay(0),
		targetId(INVALID_ID) {
	type = setType;
    SetHealth(type->maxHealth);
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
		sceneNode->setPosition(Pos());
		sceneNode->setRotation(lookHeading);
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
void PlayerCharacter :: SetMoveHeading(vector3df setMoveHeading) {
	moveHeading = setMoveHeading;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: SetLookHeading(vector3df setLookHeading) {
	lookHeading = setLookHeading;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateVelocity(float timeDelta) {
	vel += moveHeading * acc * timeDelta;

    // Cap the velocity at the maximum speed:
    if(vel.getLength()  > maxSpeed) {
        vel = vel.normalize();
		vel *= maxSpeed;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdatePosition(float timeDelta) {
	
	float sizeX;
	float sizeZ;

	if(vel.X > 0) {
		sizeX = 0.2;
	}
	else if(vel.X < 0) {
		sizeX = -0.2;
	}
	else
		sizeX = 0;

	if(vel.Z > 0) {
		sizeZ = 0.2;
	}
	else if(vel.Z < 0) {
		sizeZ = -0.2;
	}
	else {
		sizeZ = 0;
	}

	vector3df newPos = Pos();

	// position along the x-axis:
	if(!game->IsSolid(	Pos() + vector3df(sizeX, 0.0f,  0.1),
						Pos() + vector3df(sizeX, 0.0f,  0.1) + (vel.X * timeDelta),
						0)
	&& !game->IsSolid(	Pos() + vector3df(sizeX, 0.0f, -0.1),
						Pos() + vector3df(sizeX, 0.0f, -0.1) + (vel.X * timeDelta),
						0)) {

		newPos.X += vel.X * timeDelta;
	}
	else {
		vel.X = 0;
	}

	// position along the z-axis:
	if(!game->IsSolid(	Pos() + vector3df(0.1, 0.0f,  sizeZ),
						Pos() + vector3df(0.1, 0.0f,  sizeZ) + (vel.Z * timeDelta),
						0)
	&& !game->IsSolid(	Pos() + vector3df(-0.1, 0.0f, sizeZ),
						Pos() + vector3df(-0.1, 0.0f, sizeZ) + (vel.Z * timeDelta),
						0)) {

		newPos.Z += vel.Z * timeDelta;
	}
	else {
		vel.Z = 0;
	}

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
    return moveHeading;
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
bool PlayerCharacter :: RespawnMe() const {
	return respawnMe;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: RespawnTime() const {
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
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_POSZ], &sendPos.Z, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_VELX], &vel.X, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_VELZ], &vel.Z, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_HEALTH], &sendHealth, 4);

    return PACKET_WRITE_PLAYER_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[]) {
	vector3df readPos;
    Uint32 readHealth;

    memcpy(&readPos.X, &data[dataReadPos + PACKET_WRITE_PLAYER_POSX], 4);
    memcpy(&readPos.Z, &data[dataReadPos + PACKET_WRITE_PLAYER_POSZ], 4);
    memcpy(&vel.X, &data[dataReadPos + PACKET_WRITE_PLAYER_VELX], 4);
    memcpy(&vel.Z, &data[dataReadPos + PACKET_WRITE_PLAYER_VELZ], 4);
    memcpy(&readHealth, &data[dataReadPos + PACKET_WRITE_PLAYER_HEALTH], 4);

	SetPos(readPos);
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
 Uint32 PlayerCharacter :: TargetId() const {
 	return targetId;
 } // ----------------------------------------------------------------------------------------------




 // ------------------------------------------------------------------------------------------------
 vector3df PlayerCharacter :: LookHeading() const {
 	return lookHeading;
 } // ----------------------------------------------------------------------------------------------