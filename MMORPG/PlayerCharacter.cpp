#include "PlayerCharacter.h"
#include "GameBase.h"
#include "PacketProtocol.h"
#include "CharacterController.h"
#include <math.h>
#include <assert.h>




// ------------------------------------------------------------------------------------------------
PlayerCharacter :: PlayerCharacter(Uint32 setId, GameBase* setGame, CharacterType* setType)
    :	EntityBase(setId, setGame),
        vel(0,0),
        acc(setType->acc),
        maxSpeed(setType->maxSpeed),
		attackDelay(0),
		targetId(INVALID_ID) {
	type = setType;
    SetHealth(type->maxHealth);
    numOfBullets = NUM_OF_STARTING_BULLETS;
    numOfMissiles = 0;
    numOfMines = 0;
    score = 0;
	controller = 0;
	sceneNode = sceneManager->addCubeSceneNode(1.0f, 0, -1, vector3df(0,0,0));
	sceneNode->setMaterialFlag(EMF_LIGHTING, true);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
PlayerCharacter :: ~PlayerCharacter() {

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Spawn(Vector2df atPos) {
    Activate();
    SetPos(atPos);
    SetHealth(type->maxHealth);
    attackDelay = 0;
    vel.Set(0,0);
    acc = type->acc;
    maxSpeed = type->maxSpeed;
    targetId = INVALID_ID;
    numOfBullets = NUM_OF_STARTING_BULLETS;
    numOfMissiles = 20;
    numOfMines = 20;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Update(float timeDelta) {
    if(IsActive() && !IsDead()) {
        UpdateVelocity(timeDelta);
        UpdatePosition(timeDelta);
        UpdateFriction(timeDelta);
        UpdateAttack(timeDelta);
		controller->Update();
		sceneNode->setPosition(vector3df(Pos().x / 10, 0, Pos().y / 10));
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Draw(Vector2df camPos) const {

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
    if(controller->MoveHeading().y < 0) {
        vel.y -= acc * timeDelta;
    }
    else if(controller->MoveHeading().y > 0) {
        vel.y += acc * timeDelta;
    }

    if(controller->MoveHeading().x < 0) {
        vel.x -= acc * timeDelta;
    }
    else if(controller->MoveHeading().x > 0) {
        vel.x += acc * timeDelta;
    }

    // Cap the velocity at the maximum speed:
    if(vel.Length()  > maxSpeed) {
        vel = vel.Normal() * maxSpeed;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdatePosition(float timeDelta) {
    float charTop = Pos().y - type->height / 2;
    float charBottem = Pos().y + type->height / 2;
    float charLeft = Pos().x - type->width / 2;
    float charRight = Pos().x + type->width / 2;
	Vector2df newPos = Pos();

    // Handle velocity to the right:
    if(vel.x > 0) {
        if(!Game()->CheckCollisionWithLevel( Vector2df(charRight + vel.x * timeDelta, charTop + 2))
		&& !Game()->CheckCollisionWithLevel( Vector2df(charRight + vel.x * timeDelta, charBottem - 2))) {

            newPos.x += vel.x * timeDelta;
        }
        else {
            vel.x = 0;
        }
    }
    // Handle velocity to the left:
    else if(vel.x < 0) {
        if(!Game()->CheckCollisionWithLevel(Vector2df(charLeft + vel.x * timeDelta, charTop + 2))
		&& !Game()->CheckCollisionWithLevel(Vector2df(charLeft + vel.x * timeDelta, charBottem - 2))) {
            newPos.x += vel.x * timeDelta;
        }
        else {
            vel.x = 0;
        }
    }

    // Handle velocity down:
    if(vel.y > 0) {
        if(!Game()->CheckCollisionWithLevel(Vector2df(charRight - 2, charBottem + vel.y * timeDelta))
		&& !Game()->CheckCollisionWithLevel(Vector2df(charLeft + 2, charBottem + vel.y * timeDelta))) {
            newPos.y += vel.y * timeDelta;
        }
        else {
            vel.y = 0;
        }
    }
    // Handle velocity up
    else if(vel.y < 0) {
        if(!Game()->CheckCollisionWithLevel(Vector2df(charRight - 2, charTop + vel.y * timeDelta))
		&& !Game()->CheckCollisionWithLevel(Vector2df(charLeft + 2, charTop + vel.y * timeDelta))) {
            newPos.y += vel.y;
        }
        else {
            vel.y = 0;
        }
    }

    SetPos(newPos);
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateFriction(float timeDelta) {
    vel.x *= powf(type->friction, timeDelta);
    vel.y *= powf(type->friction, timeDelta);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateAttack(float timeDelta) {

    if(attackDelay > 0) {
        attackDelay -= timeDelta;
    }
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
Vector2df PlayerCharacter :: Heading() const {
    return controller->MoveHeading();
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
bool PlayerCharacter :: IsSolid(Vector2df atPos) const {
    return 	atPos.x > (Uint32)Pos().x - type->width / 2
            &&		atPos.x < (Uint32)Pos().x + type->width / 2
            &&		atPos.y > (Uint32)Pos().y - type->height / 2
            &&		atPos.y < (Uint32)Pos().y + type->height / 2;
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
void PlayerCharacter :: AddBullets(Uint32 amount) {
    numOfBullets += amount;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AddMissiles(Uint32 amount) {
    numOfMissiles += amount;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AddMines(Uint32 amount) {
    numOfMines += amount;
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
void PlayerCharacter :: AddPoint() {
	score++;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: GetNumOfBullets() const {
	return numOfBullets;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: GetNumOfMissiles() const {
	return numOfMissiles;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: GetNumOfMines() const {
	return numOfMines;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: GetScore() const {
	return score;
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
	Vector2df sendPos = Pos();
	Uint32 sendHealth = Health();

    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_ID], &sendId,4 );
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_POSX], &sendPos.x, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_POSY], &sendPos.y, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_VELX], &vel.x, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_VELY], &vel.y, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_HEALTH], &sendHealth, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_NUM_BULLETS], &numOfBullets, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_NUM_MISSILE], &numOfMissiles, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_NUM_MINES], &numOfMines, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_SCORE], &score, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_CONTROL_STATE], controller, sizeof(CharacterController));

    return PACKET_WRITE_PLAYER_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[]) {
	Vector2df readPos;
    CharacterController readController(this);
    Uint32 readHealth;

    memcpy(&readPos.x, &data[dataReadPos + PACKET_WRITE_PLAYER_POSX], 4);
    memcpy(&readPos.y, &data[dataReadPos + PACKET_WRITE_PLAYER_POSY], 4);
    memcpy(&vel.x, &data[dataReadPos + PACKET_WRITE_PLAYER_VELX], 4);
    memcpy(&vel.y, &data[dataReadPos + PACKET_WRITE_PLAYER_VELY], 4);
    memcpy(&readHealth, &data[dataReadPos + PACKET_WRITE_PLAYER_HEALTH], 4);
    memcpy(&numOfBullets, &data[dataReadPos + PACKET_WRITE_PLAYER_NUM_BULLETS], 4);
    memcpy(&numOfMissiles, &data[dataReadPos + PACKET_WRITE_PLAYER_NUM_MISSILE], 4);
    memcpy(&numOfMines, &data[dataReadPos + PACKET_WRITE_PLAYER_NUM_MINES], 4);
    memcpy(&score, &data[dataReadPos + PACKET_WRITE_PLAYER_SCORE], 4);
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

