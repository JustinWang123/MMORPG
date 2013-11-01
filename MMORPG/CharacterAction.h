#ifndef ACTION_H
#define ACTION_H

#include <vector>
#include <string>
class PlayerCharacter;
class CharacterActionEffect;
class CharacterActionCondition;

enum CharacterActionTypes {
	BASIC_MELEE_ATTACK,
	NUM_OF_CHARACTER_ACTION_TYPES
};

class CharacterAction {
public:
	CharacterAction(std::string setName);

	void Activate(PlayerCharacter* actor);
	void AddEffect(CharacterActionEffect* effect);
	void AddCondition(CharacterActionCondition* condition);

	std::string Name() const;

private:
	std::string name;
	std::vector<CharacterActionEffect*> effects;
	std::vector<CharacterActionCondition*> conditions;
};
#endif
