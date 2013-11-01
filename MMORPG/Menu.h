#ifndef MENU_H
#define MENU_H

#include "Irr_Wrapper.hpp"

using namespace std;

enum ChoiceType {
    CHOICE_NONE,
    CHOICE_JOIN,
    CHOICE_HOST,
    CHOICE_EDIT,
    CHOICE_EXIT
};

class Menu {
public:
    Menu();
    ~Menu();

    void Update();

    ChoiceType GetChoice();

private:
	IGUIWindow*				mainMenuWindow;
    ChoiceType choice;
};

#endif
