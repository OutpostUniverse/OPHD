#pragma once

#include "Morale.h"
#include "PopulationTable.h"

#include <vector>


class Population
{
public:
	int birthCount() const { return mBirthCount; }
	int deathCount() const { return mDeathCount; }

	void clear();

	int size() const { return mPopulation.size(); }
	int size(PopulationTable::Role role) const { return mPopulation.size(role); }

	void addPopulation(const PopulationTable& population);
	void addPopulation(PopulationTable::Role role, int count);

	int update(int morale, int food, int residences, int universities, int nurseries, int hospitals);

	void starveRate(float rate) { mStarveRate = rate; }

private:
	void spawnChildren(int morale, int residences, int nurseries);
	void spawnStudents();
	void spawnAdults(int universities);
	void spawnRetiree();

	void killChildren(int morale, int nurseries);
	void killStudents(int morale, int hospitals);
	void killAdults(PopulationTable::Role role, int morale, int hospitals);

	int consumeFood(int food);


	using MoraleModifiers = std::array<MoraleModifier, 5>;


	int mBirthCount{0};
	int mDeathCount{0};

	float mStarveRate{0.5f}; /**< Amount of population that dies during food shortages in percent. */

	PopulationTable mPopulation{}; /**< Current population. */
	PopulationTable mPopulationGrowth{}; /**< Population growth table. */
	PopulationTable mPopulationDeath{}; /**< Population death table. */
};
