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
	~Population();

	int size();
	int size(PersonRole);

	int birthCount() const { return mBirthCount; }
	int deathCount() const { return mDeathCount; }

	void clear();

	void addPopulation(PersonRole role, uint32_t count);

	int update(int morale, int food, int residences, int universities, int nurseries, int hospitals);

	void starveRate(float r) { mStarveRate = r; }

protected:

private:
	void init();
	void clearPopulationList(PersonRole);

	void spawn_children(int morale, int residences, int nurseries);
	void spawn_students();
	void spawn_adults(int universities);
	void spawn_retiree();

	void kill_children(int morale, int nurseries);
	void kill_students(int morale, int hospitals);
	void kill_adults(Population::PersonRole role, int morale, int hospitals);

	uint32_t consume_food(uint32_t _food);
	uint32_t killPopulation(Population::PersonRole _pr, size_t count);

private:
	typedef std::array<uint32_t, 5>		PopulationTable;
	typedef std::array<MoraleModifier, 5> MoraleModifiers;

private:
	uint32_t			mBirthCount;				/**<  */
	uint32_t			mDeathCount;				/**<  */

	float				mStarveRate;				/**< Amount of population that dies during food shortages in percent. */

	PopulationTable		mPopulation;				/**< Current population. */
	PopulationTable		mPopulationGrowth;			/**< Population growth table. */
	PopulationTable		mPopulationDeath;			/**< Population death table. */

	MoraleModifiers		mModifiers;					/**< Morale modifier table */
};
