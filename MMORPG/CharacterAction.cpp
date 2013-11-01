#include "CharacterAction.h"
#include "PlayerCharacter.h"
#include "CharacterActionEffect.h"

using namespace std;

// ------------------------------------------------------------------------------------------------
CharacterAction :: CharacterAction(std::string setName)
	:	name(setName) {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void CharacterAction :: Activate(PlayerCharacter* actor) {
	for(vector<CharacterActionEffect*>::iterator it = effects.begin(); it != effects.end(); it++) {
		(*it)->Process(actor);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void CharacterAction :: AddEffect(CharacterActionEffect* effect) {
	effects.push_back(effect);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void CharacterAction :: AddCondition(CharacterActionCondition* condition) {
	conditions.push_back(condition);
} // ------------------------------------------------------------------------------------------------
