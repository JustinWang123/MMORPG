#include "Irr_Wrapper.hpp"
#include <fstream>

IrrlichtDevice* 			device;
IVideoDriver* 				driver;
ISceneManager* 				sceneManager;
IGUIEnvironment* 			GUI;
MyEventReceiver* 			receiver;
ISceneCollisionManager* 	collisionManager;
ICameraSceneNode* 			camera;
IGUIFont* 					font;

// ------------------------------------------------------------------------------------------------
void  IrrInit() {

	receiver = new MyEventReceiver();

	// Setup irrlicht devices:
	SIrrlichtCreationParameters parameters;
	parameters.AntiAlias = 2;
	parameters.DriverType = EDT_OPENGL;
	parameters.WindowSize = dimension2d<u32>(800, 600);
	parameters.Bits = 32;
	parameters.Fullscreen = false;
	parameters.Stencilbuffer = false;
	parameters.Vsync = false;
	parameters.EventReceiver = receiver;
	device = createDeviceEx( parameters);
 
    driver = device->getVideoDriver();
    sceneManager = device->getSceneManager();
    GUI = device->getGUIEnvironment();
	collisionManager = sceneManager->getSceneCollisionManager();

  	IGUISkin* mySkin = GUI->getSkin();

	for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
        video::SColor col = mySkin->getColor((EGUI_DEFAULT_COLOR)i);
        col.setAlpha(255);
        mySkin->setColor((EGUI_DEFAULT_COLOR)i, col);
    }


	// Setup camera:
	camera = sceneManager->addCameraSceneNode();

	// Load the font:
}






void IrrClose() {
	SDL_Quit();
	device->drop();
}





void IrrEndScene() {
	GUI->drawAll();
	driver->endScene();
	driver->beginScene(true, true, SColor(0,0,0,0));
	sceneManager->drawAll();
}




void IrrDrawText(std::string text, int x, int y) {
	font->draw(text.c_str(), rect<s32>(x,y,640,480),SColor(255,255,255,255));
}





int IrrGetAngle(double xpos1, double ypos1, double xpos2, double ypos2) {
	int angle = 0;

	if(ypos2 < ypos1) {
		angle = (int)(90 - atan((double)(xpos2 - xpos1) / (double)(ypos1 - ypos2) ) * 180/PI);
	}
	else if(ypos2 > ypos1) {
		angle = (int)(270 - atan((double)(xpos2 - xpos1) / (double)(ypos1 - ypos2) ) * 180/PI);
	}
	else {
		if(xpos2 > xpos1)
			angle = 0;
		else
			angle = 180;
	}

	return (int)IrrWrapValue(angle + 90);
}




float IrrWrapValue(float angle) {
	if(angle > 360) {
		angle = angle - 360;
	}
	else if(angle < 0) {
		angle = 360 + angle;
	}

	return angle;
}







// EVENT RECEIVER METHODS:
// ------------------------------------------------------------------------------------------------
MyEventReceiver :: MyEventReceiver() {
	for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
	 	keyIsDown[i] = false;

	for (u32 i = 0; i < MAX_BUTTONS; ++i)
	 	guiButtonClicked[i] = false;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool MyEventReceiver :: OnEvent(const SEvent& event) {
	// KEYBOARD EVENTS:
	if (event.EventType == EET_KEY_INPUT_EVENT) {

		if(event.KeyInput.PressedDown == true) {
			keyIsDown[event.KeyInput.Key] = true;
		}
		else {

			keyIsDown[event.KeyInput.Key] = false;
		}

		keyIsDown[KEY_LSHIFT] = event.KeyInput.Shift;
		keyIsDown[KEY_LCONTROL] = event.KeyInput.Control;
	}

	// MOUSE EVENTS:
	if (event.EventType == EET_MOUSE_INPUT_EVENT) {
		keyIsDown[KEY_LBUTTON] = event.MouseInput.isLeftPressed();
		keyIsDown[KEY_RBUTTON] = event.MouseInput.isRightPressed();
		mouseX = event.MouseInput.X;
		mouseY = event.MouseInput.Y;
	}

	// GUI EVENTS:
	if (event.EventType == EET_GUI_EVENT) {
		guiEvents[event.GUIEvent.EventType] = true;

		if(event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
			guiButtonClicked[event.GUIEvent.Caller->getID()] = true;
		}
	}

	return false;
}







bool MyEventReceiver :: KeyIsDown(EKEY_CODE keyCode) const {
	return keyIsDown[keyCode];
}


bool MyEventReceiver :: GUIButtonClicked(int gui_id) {
	bool temp = guiButtonClicked[gui_id];
	guiButtonClicked[gui_id] = false;
	return temp;
}

bool MyEventReceiver :: GUIEvent(EGUI_EVENT_TYPE type) {
	bool temp = guiEvents[type];
	guiEvents[type] = false;
	return temp;
}

int MyEventReceiver :: MouseX() {
	return mouseX;
}


int MyEventReceiver :: MouseY() {
	return mouseY;
}

