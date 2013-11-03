#include "CharacterControllerClient.h"
#include "GameClient.h"
#include "Irr_Wrapper.hpp"

CharacterControllerClient :: CharacterControllerClient(PlayerCharacter* setCharacter)
	:	CharacterController(setCharacter),
		game((GameClient*)setCharacter->Game()) {
}

void CharacterControllerClient :: Update() {
	vector3df setMoveHeading(0,0, 0);
	vector3df setLookHeading = character->LookHeading();

	if(receiver->KeyIsDown(KEY_KEY_W)) {
		setMoveHeading.Z -= 1;
	}
	if(receiver->KeyIsDown(KEY_KEY_S)) {
		setMoveHeading.Z += 1;
	}
	if(receiver->KeyIsDown(KEY_KEY_A) ){
		setMoveHeading.X -= 1;
	}
	if(receiver->KeyIsDown(KEY_KEY_D)) {
		setMoveHeading.X += 1;
	}

	if(receiver->KeyIsDown(KEY_KEY_Q)) {
		setLookHeading.rotateXZBy(10);
	}
	if(receiver->KeyIsDown(KEY_KEY_E)) {
		setLookHeading.rotateXZBy(-10);
	}

	moveHeading = setMoveHeading;
	character->SetMoveHeading(setMoveHeading);

	lookHeading = setLookHeading;
	character->SetLookHeading(setLookHeading);

	doAction1 = receiver->KeyIsDown(KEY_LBUTTON);
	doAction2 = receiver->KeyIsDown(KEY_MBUTTON);
	doAction3 = receiver->KeyIsDown(KEY_RBUTTON);

	if(receiver->KeyIsDown(KEY_LBUTTON)) {
		//Uint32 targetId = game->GetCharacterIdAtPos(game->MouseWorldPos());
		//character->SetTargetId(targetId);
	}

    hasChanged = true;
}
