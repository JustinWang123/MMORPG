#include "GameServer.h"
#include "GameClient.h"
#include "GameEditor.h"
#include "SDLWrapper.h"
#include <iostream>
#include "Menu.h"
#include "Irr_Wrapper.hpp"
#include "GUI.h"

const int FRAMES_PER_SECOND = 60;
const int FRAME_TIME = 20;

enum GameMode {
    GAME_MODE_MENU,
    GAME_MODE_GameClient,
    GAME_MODE_SERVER,
    GAME_MODE_EDITOR
};

GameMode 	gameMode;
Menu* 		menu;
GameServer* server;
GameClient* gameClient;
GameEditor*	gameEditor;
ILightSceneNode *light;
IGUIWindow* mainMenuWindow;

void		InitGameModeGameClient();
void		InitGameModeGameServer();
void		InitGameModeEditor();
void		InitGameModeMenu();
void		CleanUp();

int main(int argc, char *argv[]) {
	IrrInit();

	// Setup level:
	IMeshSceneNode *origin = sceneManager->addCubeSceneNode(1.0f, 0, -1, vector3df(0,0,0));
	camera->setPosition(vector3df(50, 50, 100));
	camera->setTarget(vector3df(50, 0, 50));
	light = sceneManager->addLightSceneNode(0, vector3df(50,50,50));

	// Setup GUI:
	mainMenuWindow = GUI->addWindow(rect<s32>(600,0, 800,600), true, L"Main Menu", 0, GAME_USER_INTERFACE_WINDOW);
	mainMenuWindow->setDraggable(false);
	mainMenuWindow->getCloseButton()->setEnabled(false);
	mainMenuWindow->getCloseButton()->setVisible(false);
	GUI->addButton(core::rect<s32>(75,495,125, 510), mainMenuWindow, MAIN_MENU_JOIN_BUTTON, L"Join");
	GUI->addButton(core::rect<s32>(75,520,125, 535), mainMenuWindow, MAIN_MENU_SERVER_BUTTON, L"Host");
	GUI->addButton(core::rect<s32>(75,545,125, 560), mainMenuWindow, MAIN_MENU_EDITOR_BUTTON, L"Editor");
	GUI->addButton(core::rect<s32>(75,570,125, 585), mainMenuWindow, MAIN_MENU_QUIT_BUTTON, L"Quit");


    InitSDL(10 + BUTTON_WIDTH + 10, EXIT_POS_Y + BUTTON_HEIGHT + 10, "Gravity");
	
    if(argc > 1 && strcmp(argv[1], "0") == 0) {
        InitGameModeGameServer();
    }
    else if(argc > 1 && strcmp(argv[1], "1") == 0) {
        InitGameModeGameClient();
    }
    else {
        menu = new Menu();
    }

    int startTime = SDL_GetTicks();
    int frameTime;

    while(!quit) {
        startTime = SDL_GetTicks();

        HandleInput();

        if(gameMode == GAME_MODE_SERVER) {
            server->Update();
            server->Draw();
        }
        else if(gameMode == GAME_MODE_GameClient) {

            gameClient->Update();
            gameClient->Draw();

            if(gameClient->GetExit()) {
                InitGameModeMenu();
            }
        }
        else if(gameMode == GAME_MODE_EDITOR) {
            gameEditor->Update();
            gameEditor->Draw();

            if(gameEditor->GetExit()) {
                InitGameModeMenu();
            }
        }
        else if(gameMode == GAME_MODE_MENU) {
            menu->Update();
            menu->Draw();

            if(menu->GetChoice() == CHOICE_EXIT) {
                exit(0);
            }
            else if(menu->GetChoice() == CHOICE_JOIN) {
                InitGameModeGameClient();
            }
            else if(menu->GetChoice() == CHOICE_HOST) {
                InitGameModeGameServer();
            }
            else if(menu->GetChoice() == CHOICE_EDIT) {
                InitGameModeEditor();
            }
        }

        RenderScreen();
		IrrEndScene();

        frameTime = SDL_GetTicks() - startTime;
        if(frameTime < FRAME_TIME) {
            SDL_Delay(FRAME_TIME - frameTime);
        }
    }


    CleanUp();
    CloseSDL();
	IrrClose();
    return 0;
}

void InitGameModeMenu() {
	SetWindowSize(10 + BUTTON_WIDTH + 10, EXIT_POS_Y + BUTTON_HEIGHT + 10);
    CleanUp();
    gameMode = GAME_MODE_MENU;

    menu = new Menu();
	mainMenuWindow->setVisible(true);
}

void InitGameModeGameClient() {
	SetWindowSize(800, 600);
    CleanUp();
    gameMode = GAME_MODE_GameClient;

    gameClient = new GameClient();
	mainMenuWindow->setVisible(false);
}

void InitGameModeGameServer() {
	SetWindowSize(800, 600);
    CleanUp();
    gameMode = GAME_MODE_SERVER;

    server = new GameServer();
	mainMenuWindow->setVisible(false);
}

void InitGameModeEditor() {
	SetWindowSize(800, 600);
    CleanUp();
    gameMode = GAME_MODE_EDITOR;

    gameEditor = new GameEditor();
	mainMenuWindow->setVisible(false);
}

void CleanUp() {
    if(gameClient != 0) {
        delete gameClient;
        gameClient = 0;
    }

    if(server != 0) {
        delete server;
        server = 0;
    }

    if(gameEditor != 0) {
        delete gameEditor;
        gameEditor = 0;
    }

    if(menu != 0) {
        delete menu;
        menu = 0;
    }
}
