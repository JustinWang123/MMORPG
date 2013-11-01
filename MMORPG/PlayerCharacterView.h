#ifndef PLAYER_CHARACTER__VIEW_H
#define PLAYER_CHARACTER__VIEW_H

#include "Irr_Wrapper.hpp"
#include "BaseView.h"
class PlayerCharacter;
class GameBase;

class PlayerCharacterView : public BaseView {
public:
							PlayerCharacterView(GameBase*	setGame, PlayerCharacter* setCharacter);
							~PlayerCharacterView();

	virtual void			Update();
	virtual void			Draw();

private:
	void					DrawHUD();
	void					DrawScoreBoard();
	GameBase*				game;
	PlayerCharacter*		pc;
	IGUIWindow*				window;
	IGUIListBox*			chatWindow;
};

#endif