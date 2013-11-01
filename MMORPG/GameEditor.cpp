#include "GameEditor.h"
#include "GUI.h"


GameEditor :: GameEditor()
    : GameBase() {
    exit = false;

	// Setup GUI:
	menuWindow = GUI->addWindow(rect<s32>(600,0, 800,600), true, L"Editor Menu", 0, EDITOR_WINDOW);
	menuWindow->setDraggable(false);
	menuWindow->getCloseButton()->setEnabled(false);
	menuWindow->getCloseButton()->setVisible(false);
	GUI->addButton(core::rect<s32>(75,495,125, 510), menuWindow, EDITOR_SAVE_BUTTON, L"Save");
	GUI->addButton(core::rect<s32>(75,520,125, 535), menuWindow, EDITOR_QUIT_BUTTON, L"Join");
}


GameEditor :: ~GameEditor() {
    menuWindow->setVisible(false);
}

void GameEditor :: Update() {
    // Exit:
    if(receiver->GUIButtonClicked(EDITOR_QUIT_BUTTON)) {
        exit = true;
    }

    // Save Level:
    if(receiver->GUIButtonClicked(EDITOR_SAVE_BUTTON)) {
        gameMap->SaveMap();
    }
}
