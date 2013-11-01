#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H

#include "Vector2df.h"
#include "PlayerCharacter.h"
class GameBase;

class CharacterController {
public:
								CharacterController(PlayerCharacter* setCharacter);
	virtual 					~CharacterController(){}

	virtual void				Update();

	virtual void				SetState(CharacterController* setState);

	// Accessors:
	Vector2df 					MoveHeading() const;
	Vector2df 					AimHeading() const;
	bool						DoAction1() const;
	bool						DoAction2() const;
	bool						DoAction3() const;
	bool						HasChanged();

protected:
	Vector2df 					moveHeading;
	Vector2df 					aimHeading;
	bool						doAction1;
	bool						doAction2;
	bool						doAction3;
	bool						hasChanged;
	PlayerCharacter*			character;

private:
	GameBase*					game;
};


#endif // CHARACTER_CONTROLLER_H
