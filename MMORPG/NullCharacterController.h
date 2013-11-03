#ifndef NULL_CHARACTER_CONTROLLER_H
#define NULL_CHARACTER_CONTROLLER_H

#include "CharacterController.h"

class NullCharacterController : public CharacterController {
public:
								NullCharacterController(){}

	virtual 					~NullCharacterController(){}

	virtual void				Update() {}

	virtual void				SetState(CharacterController* setState) {}

};

#endif