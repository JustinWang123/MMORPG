#include "CharacterControllerClient.h"
#include "GameClient.h"
#include "Irr_Wrapper.hpp"

// ------------------------------------------------------------------------------------------------
CharacterControllerClient :: CharacterControllerClient(PlayerCharacter* setCharacter)
	:	CharacterController(setCharacter),
		game((GameClient*)setCharacter->Game()) {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void CharacterControllerClient :: Update() {
	UpdateMovement();
	UpdateLookHeading();
	UpdateActions();
	UpdateTargetSelect();

    hasChanged = true;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void CharacterControllerClient :: UpdateMovement() {
	vector3df setMoveHeading(0, 0, 0);
	
	vector3df lookHeadingNoY = character->LookHeading();
	lookHeadingNoY.Y = 0;
	lookHeadingNoY.normalize();

	if(receiver->KeyIsDown(KEY_KEY_W)) {
		setMoveHeading += lookHeadingNoY;
	}
	if(receiver->KeyIsDown(KEY_KEY_S)) {
		setMoveHeading -= lookHeadingNoY;
	}
	if(receiver->KeyIsDown(KEY_KEY_A) ){
		setMoveHeading += lookHeadingNoY.crossProduct(vector3df(0, 1, 0));
	}
	if(receiver->KeyIsDown(KEY_KEY_D)) {
		setMoveHeading -= lookHeadingNoY.crossProduct(vector3df(0, 1, 0));
	}

	moveHeading = setMoveHeading;
	character->SetMoveHeading(setMoveHeading);
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void CharacterControllerClient :: UpdateLookHeading() {
	vector3df setLookHeading = character->LookHeading();

	// Rotation:
	if(receiver->KeyIsDown(KEY_KEY_Q)) {
		setLookHeading.rotateXZBy(10);
	}
	if(receiver->KeyIsDown(KEY_KEY_E)) {
		setLookHeading.rotateXZBy(-10);
	}
	static Uint32 oldMouseX;
	static Uint32 oldMouseY;

	if(receiver->KeyIsDown(KEY_RBUTTON)) {
		setLookHeading.rotateXZBy( ((Sint32)oldMouseX - (Sint32)receiver->MouseX()) / 2);
	}

	oldMouseX = receiver->MouseX();
	oldMouseY = receiver->MouseY();

	setLookHeading.normalize();
	lookHeading = setLookHeading;
	character->SetLookHeading(setLookHeading);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void CharacterControllerClient :: UpdateActions() {
	doAction1 = receiver->KeyIsDown(KEY_LBUTTON);
	doAction2 = receiver->KeyIsDown(KEY_MBUTTON);
	doAction3 = receiver->KeyIsDown(KEY_RBUTTON);

	

} // ----------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
void CharacterControllerClient :: UpdateTargetSelect() {
	if(receiver->KeyIsDown(KEY_LBUTTON)) {
		Uint32 targetId = game->EntityIdUnderScreenPos(vector2di(receiver->MouseX(), receiver->MouseY()), COLLISION_BITMASK_CHARACTER);
		character->SetTargetId(targetId);
	}
} // ----------------------------------------------------------------------------------------------
