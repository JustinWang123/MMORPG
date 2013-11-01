#include "CharacterActionCondition.h"
#include "Vector2df.h"
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
	return (actor->Pos() - Game()->GetCharacter(actor->TargetId())->Pos()).Length() < 100;
} // ----------------------------------------------------------------------------------------------
