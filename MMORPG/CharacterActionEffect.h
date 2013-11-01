#ifndef CHARACTER_ACTION_EFFECT_H
#define CHARACTER_ACTION_EFFECT_H

class PlayerCharacter;
class GameBase;

enum CharacterActionEffectTypes {
	DAMAGE_TARGET_WITH_MELEE_WEAPON,
	ADD_ACTION_DELAY,
	NUM_OF_CHARACTER_ACTION_EFFECT_TYPES
};



// ------------------------------------------------------------------------------------------------
class CharacterActionEffect {
public:
	CharacterActionEffect(GameBase* setGame);
	virtual ~CharacterActionEffect(){}

	virtual void Process(PlayerCharacter* actor)=0;

	GameBase*				Game() const;

private:
	GameBase*				game;
}; // ---------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
class DamageTargetWithMeleeWeapon : public CharacterActionEffect {
public:
	DamageTargetWithMeleeWeapon(GameBase* setGame);
	virtual void Process(PlayerCharacter* actor);

}; // ---------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
class AddActionDelayToActor : public CharacterActionEffect {
public:
	AddActionDelayToActor(GameBase* setGame);
	virtual void Process(PlayerCharacter* actor);
}; // ---------------------------------------------------------------------------------------------
#endif // CHARACTER_ACTION_EFFECT_H
