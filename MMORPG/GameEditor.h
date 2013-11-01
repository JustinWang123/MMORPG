#ifndef EDITOR_GAME_SIM_H
#define EDITOR_GAME_SIM_H

#include "GameBase.h"
#include "Irr_Wrapper.hpp"

class GameEditor : public GameBase {
public:
    GameEditor();
    virtual ~GameEditor();

	virtual void			Draw() {}

    virtual void			Update();
private:
   	IGUIWindow*				menuWindow;
};
#endif // EDITOR_GAME_SIM_H
