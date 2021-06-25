#pragma once

#include "PopulationTable.h"
#include "Morale.h"
#include <vector>


class Population
{
public:
	Population();

	int birthCount() const { return mBirthCount; }
	int deathCount() const { return mDeathCount; }

	void clear();
	
	int size() { return mPopulation.size(); }
	int size(PopulationTable::Role role) { return mPopulation.size(role); }

	void addPopulation(PopulationTable::Role role, int count);

	int update(int morale, int food, int residences, int universities, int nurseries, int hospitals);

	void starveRate(float rate) { mStarveRate = rate; }

private:
	int adults() const;

	void spawn_children(int morale, int residences, int nurseries);
	void spawn_students();
	void spawn_adults(int universities);
	void spawn_retiree();

	void kill_children(int morale, int nurseries);
	void kill_students(int morale, int hospitals);
	void kill_adults(PopulationTable::Role role, int morale, int hospitals);

	int consume_food(int food);


	using MoraleModifiers = std::array<MoraleModifier, 5>;


	int mBirthCount;
	int mDeathCount;

	float mStarveRate; /**< Amount of population that dies during food shortages in percent. */

	PopulationTable mPopulation; /**< Current population. */
	PopulationTable mPopulationGrowth; /**< Population growth table. */
	PopulationTable mPopulationDeath; /**< Population death table. */
};
