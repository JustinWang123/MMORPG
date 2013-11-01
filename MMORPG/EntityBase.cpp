#include "EntityBase.h"
#include "Map.h"
#include <assert.h>

EntityBase :: EntityBase(Uint32 setId, GameBase* setGame)
    :	id(setId),
    	isActive(false),
		game(setGame) {
}

Uint32 EntityBase :: Id() const {
    return id;
}

bool EntityBase :: IsActive() const {
    return isActive;
}

vector3df EntityBase :: Pos() const {
    return pos;
}

GameBase* EntityBase :: Game() const {
	return game;
}

// ------------------------------------------------------------------------------------------------
Uint32 EntityBase :: Health() const {
	return health;
} // ----------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
Uint32 EntityBase :: AttackerId() const {
	return attackerId;
} // ----------------------------------------------------------------------------------------------

bool EntityBase :: IsSolid(vector3df atPos) const {
	return false;
}

bool EntityBase :: IsDead() const {
	return health == 0;
}

void EntityBase :: Activate() {
    isActive = true;
}

void EntityBase :: Deactivate() {
    isActive = false;
}

void EntityBase :: SetPos(vector3df setPos) {
    pos = setPos;
}


// ------------------------------------------------------------------------------------------------
void EntityBase :: SetHealth(Uint32 setHealth) {
	assert(setHealth >= 0 && "EntityBase::SetHealth");
	health = setHealth;


} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void EntityBase :: DecreaseHealth(Uint32 setAttackerId, Uint32 amount) {
	assert(IsActive() && "EntityBase::DecreaseHealth");
    assert(amount > 0 && "EntityBase::DecreaseHealth");

	attackerId = setAttackerId;

    if(amount > health) {
        health = 0;
    }
    else {
        health -= amount;
    }
} // ----------------------------------------------------------------------------------------------

