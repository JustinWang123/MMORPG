#include "CharacterController.h"
#include "SDLWrapper.h"
#include "GameBase.h"

CharacterController :: CharacterController()
	:	moveHeading(0, 0, 0),
		lookHeading(0, 0, 0),
		doAction1(false),
		doAction2(false),
		doAction3(false),
		hasChanged(false) {
}

CharacterController :: CharacterController(PlayerCharacter* setCharacter)
	:	moveHeading(0, 0, 0),
		lookHeading(0, 0, 0),
		doAction1(false),
		doAction2(false),
		doAction3(false),
		hasChanged(false),
		character(setCharacter),
		game(setCharacter->Game()) {
}

bool CharacterController :: HasChanged() {
	bool returnValue = hasChanged;
	hasChanged = false;
	return returnValue;
}

void CharacterController :: Update() {
	character->SetMoveHeading(moveHeading);
}

void CharacterController :: SetState(CharacterController* setState) {
	moveHeading = setState->MoveHeading();
	lookHeading = setState->LookHeading();
	doAction1 = setState->DoAction1();
	doAction2 = setState->DoAction2();
	doAction3 = setState->DoAction3();
}

vector3df CharacterController :: MoveHeading() const {
	return moveHeading;
}

vector3df CharacterController :: LookHeading() const {
	return lookHeading;
}

bool CharacterController :: DoAction1() const {
	return doAction1;
}

bool CharacterController :: DoAction2() const {
	return doAction2;
}

bool CharacterController :: DoAction3() const {
	return doAction3;
}
