#include "CharacterActionEffect.h"
#include "GameBase.h"

// CharacterActionEffect:
// ------------------------------------------------------------------------------------------------
CharacterActionEffect :: CharacterActionEffect(GameBase* setGame)
	: game(setGame) {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
GameBase* CharacterActionEffect :: Game() const {
	return game;
} // ----------------------------------------------------------------------------------------------




// DamageTargetWithMeleeWeapon:
// ------------------------------------------------------------------------------------------------
DamageTargetWithMeleeWeapon :: DamageTargetWithMeleeWeapon(GameBase* setGame)
	: 	CharacterActionEffect(setGame) {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void  DamageTargetWithMeleeWeapon :: Process(PlayerCharacter* actor) {
	PlayerCharacter* targetCharacter = Game()->GetCharacter(actor->TargetId());
	targetCharacter->DecreaseHealth(actor->Id(), 10);
} // ----------------------------------------------------------------------------------------------




// AddActionDelayToActor:
// ------------------------------------------------------------------------------------------------
AddActionDelayToActor :: AddActionDelayToActor(GameBase* setGame)
	: 	CharacterActionEffect(setGame) {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void  AddActionDelayToActor :: Process(PlayerCharacter* actor) {
	actor->AddActionDelay(50);
} // ----------------------------------------------------------------------------------------------
