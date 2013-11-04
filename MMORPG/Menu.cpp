#include "Menu.h"
#include "GUI.h"

// ------------------------------------------------------------------------------------------------
Menu :: Menu() {
    choice = CHOICE_NONE;

	// Setup GUI:
	mainMenuWindow = GUI->addWindow(rect<s32>(1400,0, 1600,600), true, L"Main Menu", 0, GAME_USER_INTERFACE_WINDOW);
	mainMenuWindow->setDraggable(false);
	mainMenuWindow->getCloseButton()->setEnabled(false);
	mainMenuWindow->getCloseButton()->setVisible(false);
	GUI->addButton(core::rect<s32>(75,495,125, 510), mainMenuWindow, MAIN_MENU_JOIN_BUTTON, L"Join");
	GUI->addButton(core::rect<s32>(75,520,125, 535), mainMenuWindow, MAIN_MENU_SERVER_BUTTON, L"Host");
	GUI->addButton(core::rect<s32>(75,545,125, 560), mainMenuWindow, MAIN_MENU_EDITOR_BUTTON, L"Editor");
	GUI->addButton(core::rect<s32>(75,570,125, 585), mainMenuWindow, MAIN_MENU_QUIT_BUTTON, L"Quit");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Menu :: ~Menu() {
	mainMenuWindow->setVisible(false);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Menu :: Update() {
	if(receiver->GUIButtonClicked(MAIN_MENU_QUIT_BUTTON)) {
        choice = CHOICE_EXIT;
    }
    else if(receiver->GUIButtonClicked(MAIN_MENU_JOIN_BUTTON)) {
        choice = CHOICE_JOIN;
    }
    else if(receiver->GUIButtonClicked(MAIN_MENU_SERVER_BUTTON)) {
        choice = CHOICE_HOST;
    }
    else if(receiver->GUIButtonClicked(MAIN_MENU_EDITOR_BUTTON)) {
        choice = CHOICE_EDIT;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
ChoiceType Menu :: GetChoice() {
    return choice;
} // ----------------------------------------------------------------------------------------------
