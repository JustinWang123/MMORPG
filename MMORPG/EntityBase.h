#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "Irr_Wrapper.hpp"
class GameBase;

class EntityBase {
public:
    EntityBase(Uint32 setId, GameBase* setGame);

    virtual void 			Update(float timeDelta)=0;

    // Accessors:
    bool					IsActive() const;
    Uint32 					Id() const;
    vector3df				Pos() const;
    GameBase*				Game() const;
	Uint32					Health() const;
	Uint32					AttackerId() const;
	virtual bool			IsSolid(vector3df atPos) const;
	bool					IsDead() const;

    // Modifiers:
    void					Activate();
    void					Deactivate();
    void					SetPos(vector3df setPos);
	void					SetHealth(Uint32 setHealth);
	void					DecreaseHealth(Uint32 setAttackerId, Uint32 amount);

private:
    Uint32 					id;
    bool					isActive;
	Uint32					health;
    vector3df				pos;
    GameBase* 				game;
    Uint32					attackerId;
};

#endif
