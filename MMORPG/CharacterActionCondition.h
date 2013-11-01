#ifndef CHARACTER_ACTION_CONDITION_H
#define CHARACTER_ACTION_CONDITION_H

class PlayerCharacter;
class GameBase;

enum CharacterActionConditionTypes {
	TARGET_IN_RANGE,
	NUM_OF_CHARACTER_ACTION_CONDITION_TYPES
};



// ------------------------------------------------------------------------------------------------
class CharacterActionCondition {
public:
	CharacterActionCondition(GameBase* setGame);
	virtual ~CharacterActionCondition(){}

	virtual bool Evaluate(PlayerCharacter* actor)=0;

	GameBase*				Game() const;

private:
	GameBase*				game;
}; // ---------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
class TargetInRangeCondition : public CharacterActionCondition{
public:
	TargetInRangeCondition(GameBase* setGame);
	virtual ~TargetInRangeCondition(){}

	virtual bool Evaluate(PlayerCharacter* actor);

};
#endif // CHARACTER_ACTION_CONDITION_H
