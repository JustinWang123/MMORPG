#ifndef CHARACTER_CONTROLLER_CLIENT_H
#define CHARACTER_CONTROLLER_CLIENT_H

#include "CharacterController.h"
#include "ToggleButton.h"

class GameClient;

class CharacterControllerClient : public CharacterController {
public:
	CharacterControllerClient(PlayerCharacter* setCharacter);

	virtual void				Update();

	virtual void				SetState(CharacterController* setState){}

private:
	GameClient*					game;
	ToggleButton				autoAttackButton;
};

#endif
