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


	Population();

	int size() const;
	int size(PersonRole) const;

	int birthCount() const { return mBirthCount; }
	int deathCount() const { return mDeathCount; }

	void clear();

	void addPopulation(PersonRole role, int count);

	int update(int morale, int food, int residences, int universities, int nurseries, int hospitals);

	void starveRate(float rate) { mStarveRate = rate; }

private:
	int adults() const;

	void spawnChildren(int morale, int residences, int nurseries);
	void spawnStudents();
	void spawnAdults(int universities);
	void spawnRetiree();

	void killChildren(int morale, int nurseries);
	void killStudents(int morale, int hospitals);
	void killAdults(Population::PersonRole role, int morale, int hospitals);

	int consumeFood(int food);


	using PopulationTable = std::array<int, 5>;
	using MoraleModifiers = std::array<MoraleModifier, 5>;


	int mBirthCount;
	int mDeathCount;

	float mStarveRate; /**< Amount of population that dies during food shortages in percent. */

	PopulationTable mPopulation; /**< Current population. */
	PopulationTable mPopulationGrowth; /**< Population growth table. */
	PopulationTable mPopulationDeath; /**< Population death table. */
};
