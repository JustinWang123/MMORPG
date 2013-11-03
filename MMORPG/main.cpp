#include "GameServer.h"
#include "GameClient.h"
#include "GameEditor.h"
#include <iostream>
#include "Menu.h"
#include "Irr_Wrapper.hpp"
#include "GUI.h"
#include "SDLWrapper.h"

const int FRAMES_PER_SECOND = 60;
const int FRAME_TIME = 20;

enum GameMode {
    GAME_MODE_MENU,
    GAME_MODE_CLIENT,
    GAME_MODE_SERVER,
    GAME_MODE_EDITOR
};

GameMode 	gameMode;
Menu* 		menu;
GameServer* server;
GameClient* gameClient;
GameEditor*	gameEditor;
ILightSceneNode *light;


void		InitGameModeGameClient();
void		InitGameModeGameServer();
void		InitGameModeEditor();
void		InitGameModeMenu();
void		CleanUp();

int main(int argc, char *argv[]) {
	IrrInit();
	SDLNet_Init();

	// Setup level:
	IMeshSceneNode *origin = sceneManager->addCubeSceneNode(1.0f, 0, -1, vector3df(0,0,0));
	camera->setPosition(vector3df(50, 50, 100));
	camera->setTarget(vector3df(50, 0, 50));
	light = sceneManager->addLightSceneNode(0, vector3df(50,50,50));

	// Join or Host from the command line:
    if(argc > 1 && strcmp(argv[1], "0") == 0) {
        InitGameModeGameServer();
    }
    else if(argc > 1 && strcmp(argv[1], "1") == 0) {
        InitGameModeGameClient();
    }
    else {
        menu = new Menu();
    }

    int startTime = device->getTimer()->getTime();
    int frameTime;

    while(device->run() && !quit)
	{
        startTime = device->getTimer()->getTime();

        if(gameMode == GAME_MODE_SERVER) {
            server->Update();
            server->Draw();
        }
        else if(gameMode == GAME_MODE_CLIENT) {

            gameClient->Update();
            gameClient->Draw();

            if(gameClient->GetExit()) {
                InitGameModeMenu();
            }
        }
        else if(gameMode == GAME_MODE_EDITOR) {
            gameEditor->Update();

            if(gameEditor->GetExit()) {
                InitGameModeMenu();
            }
        }
        else if(gameMode == GAME_MODE_MENU) {
            menu->Update();

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

		IrrEndScene();

        frameTime = device->getTimer()->getTime() - startTime;
        if(frameTime < FRAME_TIME) {
			device->sleep(FRAME_TIME - frameTime);
        }
    }


    CleanUp();
    CloseSDL();
	IrrClose();
    return 0;
}

void InitGameModeMenu() {
    CleanUp();
    gameMode = GAME_MODE_MENU;
    menu = new Menu();
}

void InitGameModeGameClient() {
    CleanUp();
    gameMode = GAME_MODE_CLIENT;
    gameClient = new GameClient();
}

void InitGameModeGameServer() {
    CleanUp();
    gameMode = GAME_MODE_SERVER;
    server = new GameServer();
}

void InitGameModeEditor() {
    CleanUp();
    gameMode = GAME_MODE_EDITOR;
    gameEditor = new GameEditor();
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
