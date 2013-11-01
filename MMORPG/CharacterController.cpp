#include "CharacterController.h"
#include "SDLWrapper.h"
#include "GameBase.h"

CharacterController :: CharacterController(PlayerCharacter* setCharacter)
	:	moveHeading(0, 0, 0),
		aimHeading(0, 0, 0),
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
}

void CharacterController :: SetState(CharacterController* setState) {
	moveHeading = setState->MoveHeading();
	aimHeading = setState->AimHeading();
	doAction1 = setState->DoAction1();
	doAction2 = setState->DoAction2();
	doAction3 = setState->DoAction3();
}

vector3df CharacterController :: MoveHeading() const {
	return moveHeading;
}

vector3df CharacterController :: AimHeading() const {
	return aimHeading;
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
