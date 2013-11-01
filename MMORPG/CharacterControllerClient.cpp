#include "CharacterControllerClient.h"
#include "GameClient.h"
#include "Irr_Wrapper.hpp"

CharacterControllerClient :: CharacterControllerClient(PlayerCharacter* setCharacter)
	:	CharacterController(setCharacter),
		game((GameClient*)setCharacter->Game()) {
}

void CharacterControllerClient :: Update() {
	vector3df setMoveHeading(0,0, 0);

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

	moveHeading = setMoveHeading;

	doAction1 = receiver->KeyIsDown(KEY_LBUTTON);
	doAction2 = receiver->KeyIsDown(KEY_MBUTTON);
	doAction3 = receiver->KeyIsDown(KEY_RBUTTON);

	float length = sqrt( pow((float)receiver->MouseX() - GAME_SCREEN_WIDTH / 2,2) + pow((float)receiver->MouseY() - GAME_SCREEN_HEIGHT / 2,2));
    float headingX = ((float) receiver->MouseX() - GAME_SCREEN_WIDTH / 2) / length;
    float headingY = ((float) receiver->MouseY() - GAME_SCREEN_HEIGHT / 2) / length;
    aimHeading = vector3df(headingX, 0, headingY);

	if(receiver->KeyIsDown(KEY_LBUTTON)) {
		//Uint32 targetId = game->GetCharacterIdAtPos(game->MouseWorldPos());
		//character->SetTargetId(targetId);
	}

    hasChanged = true;
}
