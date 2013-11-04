#include "PlayerCharacterView.h"
#include "GameBase.h"
#include "PlayerCharacter.h"
#include "GUI.h"

// ------------------------------------------------------------------------------------------------
PlayerCharacterView :: PlayerCharacterView(GameBase *setGame, PlayerCharacter *setCharacter) {
	game = setGame;
	pc = setCharacter;

	// CREATE GAME MENU WINDOW:
	window = GUI->addWindow(rect<s32>(1400,0, 1600,600), true, L"UI", 0, GAME_USER_INTERFACE_WINDOW);
	window->setDraggable(false);

	// Disable default close button on window:
	window->getCloseButton()->setEnabled(false);
	window->getCloseButton()->setVisible(false);
	
	chatWindow = GUI->addListBox(rect<s32>(20, 100, 180, 560), window, GAME_USER_INTERFACE_TEXT_WINDOW, true);
	GUI->addButton(core::rect<s32>(75,570,125, 585), window, GAME_USER_INTERFACE_QUIT_BUTTON, L"Quit");

	targetRingSceneNode = sceneManager->addCubeSceneNode(1.0f, 0, COLLISION_BITMASK_NONE, vector3df(0, 0, 0), vector3df(1, 0.01, 1));
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
PlayerCharacterView :: ~PlayerCharacterView() {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacterView :: Update() {
	camera->setTarget(pc->Pos() + vector3df(0, 5, 0));
	camera->setPosition(pc->Pos() - pc->LookHeading() * 10 + vector3df(0, 10, 0));
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacterView :: Draw() {
	DrawHUD();
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacterView :: DrawHUD() {
    DrawScoreBoard();

	if(pc->TargetId() != INVALID_ID) {
		PlayerCharacter* target = game->GetCharacter(pc->TargetId());
		targetRingSceneNode->setPosition(target->Pos());
		targetRingSceneNode->setVisible(true);
			/*
			// Draw target health:
			DrawText(	target->Pos().x - camPos.x - surfaceTargetRing->w/2 - 10,
						target->Pos().y - camPos.y - surfaceTargetRing->h/2 - 20,
						ToString(target->Health()));
			*/
	}
	else {
		targetRingSceneNode->setVisible(false);
	}

		/*
		vector3df charScreenPos = myPc->Pos() - camPos;

		// Draw health:
		DrawText(charScreenPos.x - 10, charScreenPos.y - 20, ToString(myPc->Health()));
		*/
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacterView :: DrawScoreBoard() {
	if(pc) {
       //DrawText(610,10, "Health: " + ToString(myPc->Health()));
        //DrawText(610,25, "Bullets: " + ToString(myPc->GetNumOfBullets()));
        //DrawText(700,10, "Missiles: " + ToString(myPc->GetNumOfMissiles()));
        //DrawText(700,25, "Mines: " + ToString(myPc->GetNumOfMines()));
        //DrawText(610,450, "GameServerPingTime: " + ToString(serverPingTime));
        //DrawText(610,470, "time: " + ToString( (Uint32)GetTime()));// - (Uint32)timeOfLastMessageToGameServer));
    }

    // Draw ACK list:
    /*
    string ackList;
    ackList += ToString(remoteSequenceNum);
    ackList += "       ";
    for(int i = 0; i < 32; i++) {
    	if(remoteSequenceNumBitField & (unsigned)pow(2, i)) {
    		ackList += '1';
    	}
    	else {
    		ackList += '0';
    	}
    }

    DrawText(10,10, "ACK: " + ackList);
    */


    // Draw list of players and their score:
	/*
    DrawText(590, 45, "Players:");
    int pos = 65;
    for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
        if(players[i].state != PLAYER_STATE_DISCONECTED) {
            std::string str(players[i].name);

            str += " - " + ToString(GetPlayerCharacterScore(i));

            DrawText(590, pos, str);
            pos += 20;
        }
    }
	*/

} // ----------------------------------------------------------------------------------------------
