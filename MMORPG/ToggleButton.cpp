#include "ToggleButton.h"

ToggleButton :: ToggleButton()
	: Button() {
}

void ToggleButton :: Update() {
    if((Sint32)mouseX > posX
	&& (Sint32)mouseY > posY
	&& (Sint32)mouseX < posX + upSurface->w
	&& (Sint32)mouseY < posY + upSurface->h
	&& buttonsClicked[0]
	&& !buttonHeld) {
		if(state == BUTTON_STATE_UP) {
			state = BUTTON_STATE_DOWN;
			buttonHeld = true;
		}
		else {
			state = BUTTON_STATE_UP;
			buttonHeld = true;
		}
	}

	if(buttonHeld && buttonsClicked[0]) {
		buttonHeld = false;
	}
}
