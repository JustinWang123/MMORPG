/*
IRRLICHT WRAPPER:

The purpose of the irrlicht wrapper is to hide all the low level details involved with rendering the game world, handling collision,
handling input and drawing the HUD.  The irrlicht wrapper will also contain methods for converting common data types as well as
certain math functions.

DUTIES:
Initiate all irrlicht systems
Handle Input
Render Scene
Clean Up
*/
#ifndef IRR_WRAPPER
#define IRR_WRAPPER

#include <Irrlicht.h>
#include "SDL.h"
#include <string>
#include <sstream>
class MyEventReceiver;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;

// Modified event receiver class for processing user input:

const int						MAX_BUTTONS = 100;
const int						MAX_GUI_EVENTS = EGET_COUNT;

extern IrrlichtDevice* 			device;
extern IVideoDriver* 			driver;
extern ISceneManager* 			sceneManager;
extern IGUIEnvironment* 		GUI;
extern MyEventReceiver* 		receiver;
extern ISceneCollisionManager* 	collisionManager;
extern ICameraSceneNode* 		camera;
extern IGUIFont* 				font;

// General Methods:
void 							IrrInit();
void 							IrrClose();
void							IrrEndScene();
void							IrrDrawText(std::string text, int x, int y);
int 							IrrGetAngle(double xpos1, double ypos1, double xpos2, double ypos2);
float							IrrWrapValue(float angle);


class MyEventReceiver : public IEventReceiver
{
	public:
		MyEventReceiver();
		virtual bool OnEvent(const SEvent& event);
		bool KeyIsDown(EKEY_CODE keyCode) const;
		bool GUIButtonClicked(int gui_id);
		bool GUIEvent(EGUI_EVENT_TYPE type);
		int MouseX();
		int MouseY();

	private:
        bool keyIsDown[KEY_KEY_CODES_COUNT]; // Holds the state of all keyboard and mouse keys
		bool guiButtonClicked[MAX_BUTTONS]; // Used to determine if a GUI button has been clicked
		bool guiEvents[MAX_GUI_EVENTS]; // Used to determine if a GUI event has occured
		int mouseX;
		int mouseY;
};




#endif
