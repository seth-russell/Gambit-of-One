/*
Creature.hpp

Date Last Updated: August 2, 2016

This header file was made during the Spring 2015 SFML Game Development
Tutorial at New College of Florida.  This code follows the code from the
textbook "SFML Game Development" by Artur Moreira, Henrick Vogelius
Hansson, and Jan Haller.

Creature is one of the main Entities of the game.  They fight with each
other using varied behaviors including ranged attacks and melee attacks.
Some move can move in patterns, some stand still, while others will
converge on the player when they are within a certain distance.

Updates:
	- May 25, 2016: Incorporated CombatBehavior
	- May 30, 2016: Commented out code made redundant by CombatBehavior
	- June 13, 2016: Incorporated MovementBehavior, commented out redundant code, 
					 reordered methods and members so comments were easy to find
	- July 4, 2016: Updated isMelee() and isRanged() to reflect changes in
					CreatureData
	- August 2, 2016: CreatureData is now a member, and there is a slight dependency
					  on DataTable's pointer
*/

#ifndef CREATURE_HPP
#define CREATURE_HPP

#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"
#include "Projectile.hpp"
#include "TextNode.hpp"
#include "DataStructures.h"
#include "DataTable.h"

#include <SFML/Graphics/Sprite.hpp>

class MovementBehavior; 
class CombatBehavior;

class Creature : public Entity
{
public:
	enum Type
	{
		Hero,
		Rat,
		Bandit,
		Archer,
		TypeCount
	};


public:
	Creature(Type type, const CreatureData& data, DataTable* table);

	Type							getType() const;
	virtual unsigned int			getCategory() const;
	virtual sf::FloatRect			getBoundingRect() const;
	int								getDamage() const;
	virtual bool 					isMarkedForRemoval() const;
	bool							isAllied() const;
	bool							isAggroed() const;
	bool							isGuided() const;
	bool							isRanged() const;
	bool							isMelee() const;
	bool							isAttacking() const;
	bool							isBlocked() const;
	float							getMaxSpeed() const;
	Compass							getCompass() const;
	float							getAggroDistance() const;

	void					checkAggro(sf::Vector2f position);
	void					guideTowards(sf::Vector2f position);
	void					block();
	void					fire(CommandQueue& commands);

protected:
	void					updateTexts();

private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);
	void					checkPickupDrop(CommandQueue& commands);
	void					createPickup(SceneNode& node, DataTable* table) const;
	void					createArrow(SceneNode& node, DataTable* table);
	virtual void			updateSprite();

protected:
	sf::Sprite				mSprite;
	MovementBehavior*		mMoveBehavior;
	bool 					mIsMarkedForRemoval;
	TextNode*				mHealthDisplay;

private:
	Type					mType;
	CreatureData			mData;

	CombatBehavior*			mCombatBehavior;
	Command 				mDropPickupCommand;
	sf::Vector2f			mTargetDirection;
	sf::Vector2f			mTargetPosition;
	float					mAggroDistance;

	Command 				mFireCommand;
};

struct EnemySpawn
{
	EnemySpawn(Creature::Type type, float x, float y)
		: type(type), x(x), y(y) {}
	EnemySpawn(Creature::Type type, sf::Vector2f pos)
		: type(type), x(pos.x), y(pos.y) {}

	Creature::Type	type;
	float			x;
	float			y;
};

#endif // CREATURE_HPP