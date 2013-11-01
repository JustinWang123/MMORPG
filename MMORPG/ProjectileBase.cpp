#include "ProjectileBase.h"
#include <math.h>
#include "GameBase.h"
#include "PacketProtocol.h"
#include <assert.h>

// ------------------------------------------------------------------------------------------------
ProjectileBase :: ProjectileBase(Uint32 setId, GameBase* setGame, Event* setDefaultEvent)
    : 	EntityBase(setId, setGame) {
	defaultEvent = setDefaultEvent;
	hitSolidEvent = defaultEvent;
	hitCharEvent = defaultEvent;
	outOfLifeEvent = defaultEvent;
	outOfHealthEvent = defaultEvent;
	updateEvent = defaultEvent;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetState(Uint32 setOwnerPlayerId, Vector2df setPos, Vector2df setHeading, float setSpeed, float setLife, Uint32 setHealth, bool setIsMoving) {

	SetOwnerPlayerId(setOwnerPlayerId);
    SetPos(setPos);
    SetHeading(setHeading);
    SetSpeed(setSpeed);
    SetLife(setLife);
    SetHealth(setHealth);
    SetIsMoving(setIsMoving);
} // ----------------------------------------------------------------------------------------------





void ProjectileBase :: SetType(ProjectileType setType, float setScale, SDL_Surface* setSurface) {
	SetType(setType);
	SetScale(setScale);
	surface = setSurface;
    radius = surface->w / 2;

	// Remove any events:
	hitSolidEvent = defaultEvent;
	hitCharEvent = defaultEvent;
	outOfLifeEvent = defaultEvent;
	outOfHealthEvent = defaultEvent;
	updateEvent = defaultEvent;
}


// ------------------------------------------------------------------------------------------------
void ProjectileBase :: Update(float timeDelta) {
    // Pre-Conditions:
    assert(IsActive() && "ProjectileBase::Update IsActive()");

    if(IsActive()) {
		updateEvent->Process(this);

		if(isMoving) {
			Vector2df nextPos = Pos() + Heading() * speed * timeDelta;

			if(Game()->CheckCollisionWithChars(nextPos + Heading() * radius)) {
				hitPos = nextPos + Heading() * radius;
				hitCharEvent->Process(this);
			}

			if(Game()->IsPosSolid(nextPos + Heading() * radius)) {
				hitPos = nextPos + Heading() * radius;
				hitSolidEvent->Process(this);
			}

			SetPos(nextPos);
		}

		if(IsDead()) {
			outOfHealthEvent->Process(this);
		}

		DecreaseLife(timeDelta);
    }

    // Post-Conditions
    assert(Pos().x >= MAP_MIN_POS_X && Pos().x < MAP_MAX_POS_X && "ProjectileBase::Update Pos().x >= MAP_MIN_POS_X && Pos().x < MAP_MAX_POS_X");
    assert(Pos().y >= MAP_MIN_POS_Y && Pos().y < MAP_MAX_POS_Y && "ProjectileBase::Update Pos().y >= MAP_MIN_POS_Y && Pos().y < MAP_MAX_POS_Y");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: Draw(Vector2df camPos) const {

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 ProjectileBase :: WriteToPacket(Uint32 dataWritePos, Uint8 data[]) {
	Uint32 sendId = Id();
	ProjectileType sendType = Type();
    Vector2df sendPos = Pos();
    Uint32 sendHeadingDeg = HeadingDeg();
    float sendSpeed = Speed();
    Uint32 sendOwnerPlayerId = OwnerPlayerId();
    float sendLife = Life();
    Uint32 sendHealth = Health();
    bool sendIsMoving = IsMoving();

    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_ID], &sendId, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_TYPE], &sendType, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_POSX], &sendPos.x, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_POSY], &sendPos.y, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_HEADING], &sendHeadingDeg, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_SPEED], &sendSpeed, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_LIFE], &sendLife, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_HEALTH], &sendHealth, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_PLAYER_ID], &sendOwnerPlayerId, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_PROJECTILE_ISMOVING], &sendIsMoving, 1);

    return PACKET_WRITE_PROJECTILE_LENGTH;
} // ----------------------------------------------------------------------------------------------



 void ProjectileBase :: SetType(ProjectileType setType) {
 	type = setType;
 }

// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetOwnerPlayerId(Uint32 setOwnerPlayerId) {
	ownerPlayerId = setOwnerPlayerId;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetLife(float setLife) {
	assert(setLife >= 0.0f && "ProjectileBase::SetLife");
	life = setLife;

	if(life <= 0.0f) {
		life = 0.0f;

		outOfLifeEvent->Process(this);
	}

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: DecreaseLife(float amount) {
	assert(amount > 0.0f && "ProjectileBase::DecreaseLife");
	life -= amount;

	if(life <= 0.0f) {
		life = 0.0f;

		outOfLifeEvent->Process(this);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetSpeed(float setSpeed) {
	assert(setSpeed >= 0.0f && "ProjectileBase::SetSpeed");
	speed = setSpeed;
} // ----------------------------------------------------------------------------------------------






// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetScale(float setScale) {
	assert(setScale >= 0.0f && "ProjectileBase::SetScale");
	scale = setScale;
} // ------------------------------------------------------------------------------------------------


void ProjectileBase :: SetIsMoving(bool b) {
	isMoving = b;
}

void ProjectileBase :: SetHitSolidEvent(Event* setEvent) {
	hitSolidEvent = setEvent;
}

void ProjectileBase :: SetHitCharEvent(Event* setEvent) {
	hitCharEvent = setEvent;
}

void ProjectileBase :: SetOutOfLifeEvent(Event* setEvent) {
	outOfLifeEvent = setEvent;
}

void ProjectileBase :: SetOutOfHealthEvent(Event* setEvent) {
	outOfHealthEvent = setEvent;
}

void ProjectileBase :: SetUpdateEvent(Event* setEvent) {
	updateEvent = setEvent;
}

ProjectileType ProjectileBase :: Type() const {
	return type;
}

// ------------------------------------------------------------------------------------------------
Uint32 ProjectileBase :: OwnerPlayerId() const {
	return ownerPlayerId;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
float ProjectileBase :: Life() const {
	return life;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
float ProjectileBase :: Speed() const {
	return speed;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetHeading(Uint32 d) {
    headingDeg = d;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetHeading(Vector2df heading) {
    headingDeg = heading.Deg();
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32	ProjectileBase :: HeadingDeg() const {
	return headingDeg;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Vector2df ProjectileBase :: Heading() const {
    return Vector2df(headingDeg);
} // ----------------------------------------------------------------------------------------------








// ------------------------------------------------------------------------------------------------
float ProjectileBase :: Scale() const {
	return scale;
} // ------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
SDL_Surface* ProjectileBase :: Surface() const {
	return surface;
} // ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
bool ProjectileBase :: IsSolid(Vector2df atPos) const {
	return Vector2df(Pos() - atPos).Length() < radius;
} // ------------------------------------------------------------------------------------------------



Vector2df ProjectileBase :: HitPos() const {
	return hitPos;
}


bool ProjectileBase :: IsMoving() const {
	return isMoving;
}
