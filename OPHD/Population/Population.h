#pragma once

#include "Morale.h"

#include <array>
#include <vector>

class Population
{
public:
	enum PersonRole
	{
		ROLE_CHILD,
		ROLE_STUDENT,
		ROLE_WORKER,
		ROLE_SCIENTIST,
		ROLE_RETIRED
	};

public:
	Population();

	int size();
	int size(PersonRole);

	int birthCount() const { return mBirthCount; }
	int deathCount() const { return mDeathCount; }

	void clear();

	void addPopulation(PersonRole role, int count);

	int update(int morale, int food, int residences, int universities, int nurseries, int hospitals);

	void starveRate(float rate) { mStarveRate = rate; }

protected:

private:
	void init();

	void spawn_children(int morale, int residences, int nurseries);
	void spawn_students();
	void spawn_adults(int universities);
	void spawn_retiree();

	void kill_children(int morale, int nurseries);
	void kill_students(int morale, int hospitals);
	void kill_adults(Population::PersonRole role, int morale, int hospitals);

	int consume_food(int _food);

private:
	using PopulationTable = std::array<int, 5>;
	using MoraleModifiers = std::array<MoraleModifier, 5>;

private:
	int mBirthCount; /**<  */
	int mDeathCount; /**<  */

	float mStarveRate; /**< Amount of population that dies during food shortages in percent. */

	PopulationTable mPopulation; /**< Current population. */
	PopulationTable mPopulationGrowth; /**< Population growth table. */
	PopulationTable mPopulationDeath; /**< Population death table. */

	MoraleModifiers mModifiers; /**< Morale modifier table */
};
