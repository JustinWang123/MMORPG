#ifndef TOGGLE_BUTTON_H
#define TOGGLE_BUTTON_H

#include "Button.h"

class ToggleButton : public Button {
public:
	ToggleButton();
	virtual void Update();

private:
	bool buttonHeld;
};
#endif // TOGGLE_BUTTON_H
