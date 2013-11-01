#include "CharacterActionCondition.h"
#include "PlayerCharacter.h"
#include "GameBase.h"

// CharacterActionCondition:
// ------------------------------------------------------------------------------------------------
CharacterActionCondition :: CharacterActionCondition(GameBase* setGame)
	: game(setGame) {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
GameBase* CharacterActionCondition :: Game() const {
	return game;
} // ----------------------------------------------------------------------------------------------


// TargetInRangeCondition:
// ------------------------------------------------------------------------------------------------
TargetInRangeCondition :: TargetInRangeCondition(GameBase* setGame)
	: CharacterActionCondition(setGame) {

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool TargetInRangeCondition :: Evaluate(PlayerCharacter* actor) {
	return actor->Pos().getDistanceFrom(Game()->GetCharacter(actor->TargetId())->Pos()) < 5;
} // ----------------------------------------------------------------------------------------------
