#pragma once

#include "Morale.h"
#include "PopulationTable.h"
#include <vector>


class Population
{
public:
	Population();

	int birthCount() const { return mBirthCount; }
	int deathCount() const { return mDeathCount; }

	void clear();

	int size() const { return mPopulation.size(); }
	int size(PopulationTable::PersonRole role) const { return mPopulation.size(role); }

	void addPopulation(PopulationTable::PersonRole role, int count);

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
	void killAdults(PopulationTable::PersonRole role, int morale, int hospitals);

	int consumeFood(int food);


	using MoraleModifiers = std::array<MoraleModifier, 5>;


	int mBirthCount;
	int mDeathCount;

	float mStarveRate; /**< Amount of population that dies during food shortages in percent. */

	PopulationTable mPopulation; /**< Current population. */
	PopulationTable mPopulationGrowth; /**< Population growth table. */
	PopulationTable mPopulationDeath; /**< Population death table. */
};
