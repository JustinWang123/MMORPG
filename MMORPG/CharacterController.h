#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H

#include "PlayerCharacter.h"
class GameBase;

class CharacterController {
public:
								CharacterController();
								CharacterController(PlayerCharacter* setCharacter);
	virtual 					~CharacterController(){}

	virtual void				Update();

	virtual void				SetState(CharacterController* setState);

	// Accessors:
	vector3df 					MoveHeading() const;
	vector3df 					LookHeading() const;
	bool						DoAction1() const;
	bool						DoAction2() const;
	bool						DoAction3() const;
	bool						HasChanged();

protected:
	vector3df 					moveHeading;
	vector3df 					lookHeading;
	bool						doAction1;
	bool						doAction2;
	bool						doAction3;
	bool						hasChanged;
	PlayerCharacter*			character;

private:
	GameBase*					game;
};


#endif // CHARACTER_CONTROLLER_H
