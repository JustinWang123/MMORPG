#include "GameBase.h"
#include "PacketProtocol.h"
#include <assert.h>
#include "CharacterControllerClient.h"
#include "GUI.h"
#include "BaseView.h"

using namespace std;




// ------------------------------------------------------------------------------------------------
GameBase :: GameBase() {
	// Allocate a vector of packets for GameClient messages
    packets = SDLNet_AllocPacketV(4, PACKET_PACKETSIZE);

    nextPacketTime = device->getTimer()->getTime();

    events[DO_NOTHING] = new DoNothingEvent(this);
    events[BULLET_HIT_CHAR] = new BulletHitCharEvent(this);
    events[CREATE_SMOKE] = new CreateSmokeEvent(this);
    events[STEER_TOWARDS_CLOSEST_CHAR] = new SteerTowardsClosestCharEvent(this);
    events[STOP_MOVING] = new StopMovingEvent(this);
    events[SELF_DESTRUCT] = new SelfDestructEvent(this);
    events[DEACTIVATE] = new DeactivateEvent(this);

    // Initiate all action effects
    characterActionEffects[DAMAGE_TARGET_WITH_MELEE_WEAPON] = new DamageTargetWithMeleeWeapon(this);
    characterActionEffects[ADD_ACTION_DELAY] = new AddActionDelayToActor(this);

    // Initate all action conditions
    characterActionConditions[TARGET_IN_RANGE] = new TargetInRangeCondition(this);

    // Initiate all character actions
    characterActions[BASIC_MELEE_ATTACK] = new CharacterAction("Basic Melee Attack");
	characterActions[BASIC_MELEE_ATTACK]->AddEffect(characterActionEffects[DAMAGE_TARGET_WITH_MELEE_WEAPON]);
	characterActions[BASIC_MELEE_ATTACK]->AddEffect(characterActionEffects[ADD_ACTION_DELAY]);
	characterActions[BASIC_MELEE_ATTACK]->AddCondition(characterActionConditions[TARGET_IN_RANGE]);

	// Initiate all character types
	characterTypes.push_back(new CharacterType());
	characterTypes[0]->attackDelay = 10;
	characterTypes[0]->maxHealth = 100;
	characterTypes[0]->acc = 0.04f;
	characterTypes[0]->maxSpeed = 0.1f;
	characterTypes[0]->friction = 0.8;
	characterTypes[0]->width = 16;
	characterTypes[0]->height = 16;
	characterTypes[0]->autoAttackAction = characterActions[BASIC_MELEE_ATTACK];

	characterTypes.push_back(new CharacterType());
	characterTypes[1]->attackDelay = 10;
	characterTypes[1]->maxHealth = 100;
	characterTypes[1]->acc = 0.04f;
	characterTypes[1]->maxSpeed = 0.1f;
	characterTypes[1]->friction = 0.8;
	characterTypes[1]->width = 16;
	characterTypes[1]->height = 16;

	// Init all players:
	Uint32 id = 0;
    for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
    	playerCharacters[id] = new PlayerCharacter(id, this, characterTypes[0]);
		entities[id] = playerCharacters[id];
		characters[id] = playerCharacters[id];
		sceneNodeToIdMap[playerCharacters[id]->SceneNode()] = id;

        id++;
    }

	GetCharacter(0);

    // Init all npcs:
    for(Uint32 i = 0; i < MAX_NPCS; i++) {
    	nonPlayerCharacters[id] = new PlayerCharacter(id, this, characterTypes[1]);
		entities[id] = nonPlayerCharacters[id];
		characters[id] = nonPlayerCharacters[id];
		sceneNodeToIdMap[nonPlayerCharacters[id]->SceneNode()] = id;

		// Temp initing stuff:
		nonPlayerCharacters[id]->Activate();
		nonPlayerCharacters[id]->SetPos(vector3df(i * 1, 0, 2));
        id++;
    }

    // Init all projectiles:
    for(Uint32 i = 0; i < MAX_PROJECTILES; i++) {
    	projectiles[id] = new ProjectileBase(id, this, events[DO_NOTHING]);
    	entities[id] = projectiles[id];
    	id++;
    }

    gameMap = new Map(vector3df(4, 4, 4), this);

    oldTime = device->getTimer()->getTime();
	view = 0;

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
GameBase :: ~GameBase() {
    // delete all player characters:
    for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
        delete playerCharacters[i];
        playerCharacters[i] = 0;
    }

    // delete all projectiles:
    for(std::map<Uint32, ProjectileBase*>::const_iterator it = projectiles.begin();it != projectiles.end();it++) {
        delete it->second;
    }
    projectiles.clear();

    // delete all events:
    for(Uint32 i = 0; i < NUM_OF_EVENT_TYPES; i++) {
        delete events[i];
        events[i] = 0;
    }

    // delete all character actions:
    for(Uint32 i = 0; i < NUM_OF_CHARACTER_ACTION_TYPES; i++) {
		delete characterActions[i];
		characterActions[i] = 0;
    }

    // delete all character action events:
    for(Uint32 i = 0; i < NUM_OF_CHARACTER_ACTION_EFFECT_TYPES; i++) {
		delete characterActionEffects[i];
		characterActionEffects[i] = 0;
    }

    // delete all character action conditions:
    for(Uint32 i = 0; i < NUM_OF_CHARACTER_ACTION_CONDITION_TYPES; i++) {
		delete characterActionConditions[i];
		characterActionConditions[i] = 0;
    }

    delete gameMap;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameBase :: Update() {
    float timeDelta = (device->getTimer()->getTime() - oldTime) / 20.0f;
    oldTime = device->getTimer()->getTime();

	if(receiver->GUIButtonClicked(GAME_USER_INTERFACE_QUIT_BUTTON)) {
		quit = true;
	}
	
	if(view) {
		view->Update();
	}

    UpdateAllEntities(timeDelta);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameBase :: Draw() {
    if(view) {
		view->Draw();
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameBase :: LoadMap(std::string fileName) {
    gameMap->LoadMap(fileName);
} // ------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
Uint32 GameBase :: WriteLevelDataToPacket(Uint8 data[]) {
    return gameMap->WriteLevelDataToPacket(data);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameBase :: HandleLevelData(UDPpacket* packet) {
    gameMap->HandleLevelData(packet);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 GameBase :: WriteGameStateToPacket(Uint8 data[]) {
    Uint32 dataWritePos = PACKET_UPDATE_WORLD_DATA_START;

	// Insert all characters:
    Uint32 numOfCharters = 0;
    FOR_CHAR_IN_CHARACTERS {
		CHAR = it->second;

        if(CHAR->IsActive()) {
            dataWritePos += CHAR->WriteToPacket(dataWritePos, data);
            numOfCharters++;
        }
    }
    memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_PLAYERS], &numOfCharters, 4 );

	// Insert all projectiles:
    Uint32 numOfProjectiles = 0;
	FOR_PROJ_IN_PROJECTILES {
		PROJ = it->second;
        if(it->second->IsActive()) {
            dataWritePos += it->second->WriteToPacket(dataWritePos, data);
            numOfProjectiles++;
        }
    }
    memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_PROJECTILES], &numOfProjectiles, 4 );

    return dataWritePos;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameBase :: HandleUpdateWorld(UDPpacket* packet) {

    // FIXTHIS
    // Get server time stamp and calculate ping:
    //long time;
    ////memcpy(&time, &packet->data[PACKET_UPDATE_WORLD_TIME_STAMP], 4);
    //Uint32 timeDiff = GetTime() - time;
    //serverPingTime = timeDiff * 2;

    // We set everything to inactive and only activate those things we receive a message about:
	FOR_ENTITY_IN_ENTITIES { 
		ENTITY = it->second;
        ENTITY->Deactivate();
    }

    // Get number (amount) of each data type:
    Uint32 numOfCharacters;
    Uint32 numOfProjectiles;
    Uint32 numOfPickUps;
    memcpy(&numOfCharacters, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_PLAYERS], 4);
    memcpy(&numOfProjectiles, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_PROJECTILES], 4);
    memcpy(&numOfPickUps, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_PICK_UPS], 4);

    // Get all the data:
    Uint32 id;
    Uint32 dataReadPos = PACKET_UPDATE_WORLD_DATA_START;

    // Read Characters:
    for(Uint32 i = 0; i < numOfCharacters; i++) {
        memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_PLAYER_ID], 4);
        dataReadPos += characters[id]->ReadFromPacket(dataReadPos, packet->data);
    }

    // Read Projectiles:
    for(Uint32 i = 0; i < numOfProjectiles; i++) {
        dataReadPos += ReadProjectilePacket(dataReadPos, packet->data);
    }
} // ----------------------------------------------------------------------------------------------





Uint32 GameBase :: ReadProjectilePacket(Uint32 dataReadPos, Uint8 data[]) {
	Uint32			readId;
	ProjectileType 	readType;
	vector3df 		readPos;
    Uint32 			readHeadingDeg;
    float			readSpeed;
    Uint32 			readOwnerPlayerId;
    float 			readLife;
    Uint32 			readHealth;
    bool			readIsMoving;

	memcpy(&readId, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_ID], 		4);
	memcpy(&readType,	 		&data[dataReadPos + PACKET_WRITE_PROJECTILE_TYPE], 		4);
    memcpy(&readPos.X, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_POSX], 		4);
    memcpy(&readPos.Y, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_POSY], 		4);
    memcpy(&readHeadingDeg, 	&data[dataReadPos + PACKET_WRITE_PROJECTILE_HEADING], 	4);
    memcpy(&readSpeed, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_SPEED], 	4);
    memcpy(&readLife, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_LIFE], 		4);
    memcpy(&readHealth, 		&data[dataReadPos + PACKET_WRITE_PROJECTILE_HEALTH], 	4);
    memcpy(&readOwnerPlayerId, 	&data[dataReadPos + PACKET_WRITE_PROJECTILE_PLAYER_ID], 4);
	memcpy(&readIsMoving, 		&data[dataReadPos + PACKET_WRITE_PROJECTILE_ISMOVING], 	1);

	// set the projectiles type:
	if(projectiles[readId]->Type() != readType) {
		SetProjectileType(readId, readType);
	}

	// set the projectiles state:
	projectiles[readId]->SetState(readOwnerPlayerId, readPos, vector3df(readHeadingDeg), readSpeed, readLife, readHealth, readIsMoving);
	projectiles[readId]->Activate();

	return PACKET_WRITE_PROJECTILE_LENGTH;
}








// ------------------------------------------------------------------------------------------------
void GameBase :: DamageCharsInCircle(Uint32 attackerId, Uint32 damage, vector3df centerPos, float radius) {
    assert(attackerId < MAX_PLAYERS && "GameBase::DamageCharsInCircle attackerId < MAX_PLAYERS");

	FOR_ENTITY_IN_ENTITIES { 
		ENTITY = it->second;
        if(ENTITY->IsActive()) {
            float distance = (centerPos - ENTITY->Pos()).getLength();
            if(distance  < radius) {
                ENTITY->DecreaseHealth(attackerId, damage);
            }
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameBase :: DamageCharsAtPos(Uint32 attackerId, Uint32 damage, vector3df atPos) {
    assert(attackerId < MAX_PLAYERS && "GameBase::DamageCharsAtPos attackerId < MAX_PLAYERS");

	FOR_ENTITY_IN_ENTITIES { 
		ENTITY = it->second;
        if(ENTITY->IsActive() && ENTITY->IsSolid(atPos)) {
            ENTITY->DecreaseHealth(attackerId, damage);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameBase :: SpawnPlayerCharacter(Uint32 playerID) {
    vector3df spawnPos = vector3df(0, 0, 0);
    playerCharacters[playerID]->Spawn(spawnPos);
} // ----------------------------------------------------------------------------------------------


void GameBase :: CreateProjectile(ProjectileType type, int ownerPlayerId, vector3df setPos, vector3df setHeading) {
	FOR_PROJ_IN_PROJECTILES { 
		PROJ = it->second;
    	if(!PROJ->IsActive()) {
			SetProjectileType(PROJ->Id(), type);

			if(type == PROJECTILE_TYPE_BULLET) {
				PROJ->SetState(ownerPlayerId, setPos, setHeading, BULLET_SPEED, BULLET_LIFE, BULLET_HEALTH, true);
			}
			else if(type == PROJECTILE_TYPE_MISSILE) {
				PROJ->SetState(ownerPlayerId, setPos, setHeading, MISSILE_SPEED, MISSILE_LIFE, MISSILE_HEALTH, true);
			}
			else if(type == PROJECTILE_TYPE_MINE) {
				PROJ->SetState(ownerPlayerId, setPos, setHeading, MINE_SPEED, MINE_LIFE, MINE_HEALTH, true);
			}
			else if(type == PROJECTILE_TYPE_EXPLOSION) {
				CreateExplosion(ownerPlayerId, setPos);
			}
			else if(type == PROJECTILE_TYPE_EXPLOSION_PARTICLE) {
				PROJ->SetState(ownerPlayerId, setPos, setHeading, EXPLOSION_PARTICLE_SPEED, EXPLOSION_PARTICLE_LIFE, EXPLOSION_PARTICLE_HEALTH, true);
			}
			else if(type == PROJECTILE_TYPE_SMOKE) {
				PROJ->SetState(ownerPlayerId, setPos, setHeading, SMOKE_PARTICLE_SPEED, SMOKE_PARTICLE_LIFE, SMOKE_PARTICLE_HEALTH, true);
			}

			PROJ->Activate();

			break;
		}
    }
}

void GameBase :: SetProjectileType(Uint32 id, ProjectileType setType) {
	// Default event is do nothing:
	projectiles[id]->SetHitCharEvent(events[DO_NOTHING]);
	projectiles[id]->SetHitSolidEvent(events[DO_NOTHING]);
	projectiles[id]->SetOutOfLifeEvent(events[DO_NOTHING]);
	projectiles[id]->SetOutOfHealthEvent(events[DO_NOTHING]);
	projectiles[id]->SetUpdateEvent(events[DO_NOTHING]);

	if(setType == PROJECTILE_TYPE_BULLET) {
		projectiles[id]->SetType(setType, 1.0f, bulletSurface);
		projectiles[id]->SetHitCharEvent(events[BULLET_HIT_CHAR]);
		projectiles[id]->SetHitSolidEvent(events[BULLET_HIT_CHAR]);
		projectiles[id]->SetOutOfLifeEvent(events[DEACTIVATE]);
		projectiles[id]->SetOutOfHealthEvent(events[DEACTIVATE]);
	}
	else if(setType == PROJECTILE_TYPE_MISSILE) {
		projectiles[id]->SetType(setType, 1.0f, missileSurface);
		projectiles[id]->SetHitCharEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetHitSolidEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetOutOfLifeEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetOutOfHealthEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetUpdateEvent(events[CREATE_SMOKE]);
	}
	else if(setType == PROJECTILE_TYPE_MINE) {
		projectiles[id]->SetType(setType, 1.0f, mineSurface);
		projectiles[id]->SetHitCharEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetHitSolidEvent(events[STOP_MOVING]);
		projectiles[id]->SetOutOfLifeEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetOutOfHealthEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetUpdateEvent(events[STEER_TOWARDS_CLOSEST_CHAR]);
	}
	else if(setType == PROJECTILE_TYPE_EXPLOSION) {
		// Not really a projectile
	}
	else if(setType == PROJECTILE_TYPE_EXPLOSION_PARTICLE) {
		projectiles[id]->SetType(setType, 1.0f, explosionSurface);
		projectiles[id]->SetHitCharEvent(events[BULLET_HIT_CHAR]);
		projectiles[id]->SetHitSolidEvent(events[BULLET_HIT_CHAR]);
		projectiles[id]->SetOutOfLifeEvent(events[DEACTIVATE]);
		projectiles[id]->SetOutOfHealthEvent(events[DEACTIVATE]);
		projectiles[id]->SetUpdateEvent(events[CREATE_SMOKE]);
	}
	else if(setType == PROJECTILE_TYPE_SMOKE) {
		projectiles[id]->SetType(setType, 1.0f, smokeSurface);
		//projectiles[id]->SetHitCharEvent(events[DEACTIVATE]);
		//projectiles[id]->SetHitSolidEvent(events[DEACTIVATE]);
		projectiles[id]->SetOutOfLifeEvent(events[DEACTIVATE]);
		projectiles[id]->SetOutOfHealthEvent(events[DEACTIVATE]);
	}
}




// ------------------------------------------------------------------------------------------------
void GameBase :: CreateExplosion(int ownerPlayerID, vector3df setPos) {
    for(Uint32 i = 0; i < 360; i += 35) {
        CreateProjectile(PROJECTILE_TYPE_EXPLOSION_PARTICLE, ownerPlayerID, setPos, vector3df(i));
    }
} // ----------------------------------------------------------------------------------------------








void GameBase :: UpdateAllEntities(float timeDelta) {
	FOR_ENTITY_IN_ENTITIES { 
		ENTITY = it->second;
		if(ENTITY->IsActive()) {
			ENTITY->Update(timeDelta);
		}
	}

	UpdateRespawn(timeDelta);
}




// ------------------------------------------------------------------------------------------------
void GameBase :: UpdateRespawn(float timeDelta) {
    FOR_PC_IN_PLAYER_CHARACTERS {
		PC = it->second;
        if(PC->IsActive() && PC->IsDead()) {
            // If the player just died:
            if(!PC->RespawnMe()) {
                PC->SetRespawnMe(true);
                PC->SetRespawnTime(device->getTimer()->getTime() + RESPAWN_DELAY); // FIXTHIS timing? 

                // Add point to his killer:
                if(PC->AttackerId() < MAX_PLAYERS) {
                }

                //SendKillMessageToAll(players[i].pc->GetKillerId(), i); FIXTHIS
            }
            // If the player is already dead and is ready to respawn:
            else if(device->getTimer()->getTime() > PC->RespawnTime()) {
                PC->SetRespawnMe(false);
                SpawnPlayerCharacter(PC->Id());
            }
        }
    }
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
bool GameBase :: CheckCollisionWithChars(vector3df atPos) const {
    // Collide with characters:
    FOR_CHAR_IN_CHARACTERS {
		CHAR = it->second;
        if(CHAR->IsActive() && CHAR->IsSolid(atPos)) {
            return true;
        }
    }

    // Collide with projectiles:
	FOR_PROJ_IN_PROJECTILES {
		PROJ = it->second;
        if(PROJ->IsActive() && PROJ->IsSolid(atPos)) {
            return true;
        }
    }
    return false;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool GameBase :: IsSolid(vector3df start, vector3df end, Uint32 bitMask) {
	line3d<f32> ray;
	vector3df intersection;
	triangle3df hitTriangle;
	ISceneNode* scene_node;
	ray.start = start;
	ray.end = 	end;
	ISceneNode* node = collisionManager->getSceneNodeAndCollisionPointFromRay(ray, intersection, hitTriangle, bitMask, 0);

	return node;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
 PlayerCharacter* GameBase :: GetCharacter(Uint32 atId) {
    return characters.at(atId);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 GameBase :: GetCharacterIdAtPos(vector3df pos) {
	FOR_CHAR_IN_CHARACTERS {
		CHAR = it->second;
		if(CHAR->IsSolid(pos)) {
			return CHAR->Id();
		}
	}

	return INVALID_ID;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 GameBase :: EntityIdHitByRay(vector3df start, vector3df end, Uint32 bitMask) {
	line3d<f32> ray;
	vector3df intersection;
	triangle3df hitTriangle;
	ISceneNode* scene_node;
	ray.start = start;
	ray.end = 	end;
	ISceneNode* node = collisionManager->getSceneNodeAndCollisionPointFromRay(ray, intersection, hitTriangle, bitMask, 0);
	
	if(node) {
		return sceneNodeToIdMap.at(node);
	}
	else {
		return INVALID_ID;
	}
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
Uint32 GameBase :: EntityIdUnderScreenPos(vector2di screenPos, Uint32 bitMask) {
	ISceneNode* node = collisionManager->getSceneNodeFromScreenCoordinatesBB(screenPos, bitMask);

	if(node) {
		return sceneNodeToIdMap.at(node);
	}
	else {
		return INVALID_ID;
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool GameBase :: CheckCollisionWithLevel(vector3df atPos) const {
    assert(gameMap && "GameBase :: CheckCollisionWithLevel gameMap");
    return gameMap->IsPosSolid(atPos);
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
bool GameBase :: IsPosSolid(vector3df atPos) const {
    return  CheckCollisionWithLevel(atPos)  || CheckCollisionWithChars(atPos);
} // ----------------------------------------------------------------------------------------------








// ------------------------------------------------------------------------------------------------
vector3df GameBase :: GetClosestCharPos(vector3df atPos) const {
    int closestCharID = -1;
    float closestDistance = 100000.0f; // This large number ensures at least one char will be closer

	FOR_CHAR_IN_CHARACTERS {
		CHAR = it->second;
        if(CHAR->IsActive()) {
			float newDistance = atPos.getDistanceFrom(CHAR->Pos());

            if(newDistance < closestDistance) {
                closestDistance = newDistance;
                closestCharID = CHAR->Id();
            }
        }
    }
    if(closestCharID != -1) {
        return playerCharacters.at(closestCharID)->Pos();
    }
    else {
        return vector3df(0, 0, 0);
    }
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void GameBase :: AddMessagetoChat(std::string msg) {
	
	core::stringw msgw(msg.c_str());
	//chatWindow->addItem(msgw.c_str());

    // Get number of full lines:
    int lines =  msg.length() / CHAT_LOG_LINE_LENGTH;

    // If there is a partial line then add one to lines:
    if( (  msg.length() % CHAT_LOG_LINE_LENGTH) > 0 )
        lines++;

    // Push each line onto the chatLog:
    for(int i = 0; i < lines; i++) {
        chatLog.push_back(msg.substr(i * CHAT_LOG_LINE_LENGTH, CHAT_LOG_LINE_LENGTH));
    }
} // ----------------------------------------------------------------------------------------------



bool GameBase :: GetExit() const {
	return exit;
}
