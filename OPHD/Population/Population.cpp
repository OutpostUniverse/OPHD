#include "Population.h"

#include "../RandomNumberGenerator.h"

#include <algorithm>
#include <iostream>


namespace
{
	const int studentToScientistRate = 35;
	const int studentToAdultBase = 190;
	const int adultToRetireeBase = 2000;

	const std::array moraleModifierTable{
		MoraleModifier{50, 50, 110, 80},  // Excellent
		MoraleModifier{25, 25, 90, 75},   // Good
		MoraleModifier{0, 0, 60, 40},     // Fair
		MoraleModifier{-25, -25, 40, 20}, // Poor
		MoraleModifier{-50, -50, 20, 10}  // Terrible
	};


	/**
	 * Convenience function to cast a MoraleLevel enumerator
	 * into an array index.
	 */
	std::size_t moraleIndex(int morale)
	{
		return static_cast<std::size_t>(std::clamp(morale, 1, 999) / 200);
	}
}


/**
 * Clears entire population.
 */
void Population::clear()
{
	mPopulation = {};
}


const PopulationTable& Population::getPopulations() const
{
	return mPopulation;
}


void Population::addPopulation(const PopulationTable& population)
{
	mPopulation += population;
}


/**
 * Population check for new children.
 * 
 * \todo	Account for nurseries when implemented.
 */
void Population::spawnChildren(int morale, int residences, int nurseries)
{
	if (residences <= 0 && nurseries <= 0) { return; }

	int divisor = moraleModifierTable[moraleIndex(morale)].fertilityRate;

	mPopulationGrowth.child += mPopulation.scientist / 4 + mPopulation.worker / 2;
	int newChildren = mPopulationGrowth.child / divisor;
	mPopulationGrowth.child = mPopulationGrowth.child % divisor;

	mPopulation.child += newChildren;
	mBirthCount = newChildren;
}


void Population::spawnStudents()
{
	int divisor = std::max(mPopulation.adults(), studentToAdultBase);
	divisor = ((divisor / 40) * 3 + 16) * 4;

	mPopulationGrowth.student += mPopulation.child;
	int newStudents = mPopulationGrowth.student / divisor;
	mPopulationGrowth.student = mPopulationGrowth.student % divisor;

	mPopulation.student += newStudents;
	mPopulation.child -= newStudents;
}


void Population::spawnAdults(int universities)
{
	int divisor = std::max(mPopulation.adults(), studentToAdultBase);
	divisor = ((divisor / 40) * 3 + 45) * 4;

	mPopulationGrowth.worker += mPopulation.student;
	int newAdult = mPopulationGrowth.worker / divisor;
	mPopulationGrowth.worker = mPopulationGrowth.worker % divisor;

	// account for universities
	const auto role = (universities > 0 && randomNumber.generate(0, 100) <= studentToScientistRate) ?
		PopulationTable::Role::Scientist : PopulationTable::Role::Worker;
	mPopulation[role] += newAdult;

	mPopulation.student -= newAdult;
}


void Population::spawnRetiree()
{
	int total_adults = mPopulation.worker + mPopulation.scientist;
	int divisor = std::max(total_adults, adultToRetireeBase);
	divisor = ((divisor / 40) * 3 + 40) * 4;

	mPopulationGrowth.retiree += total_adults / 10;
	int retiree = mPopulationGrowth.retiree / divisor;
	mPopulationGrowth.retiree = mPopulationGrowth.retiree % divisor;
	mPopulation.retiree += retiree;

	/** Workers retire earlier than scientists. */
	const auto retirePopulationType = randomNumber.generate(0, 100) <= 45 ? PopulationTable::Role::Scientist : PopulationTable::Role::Worker;
	if (mPopulation[retirePopulationType] > 0) { mPopulation[retirePopulationType] -= retiree; }
}


void Population::killRole(PopulationTable::Role role, int divisor)
{
	mPopulationDeath[role] += mPopulation[role];

	int deaths = std::min(mPopulationDeath[role] / divisor, mPopulation[role]);
	mPopulationDeath[role] = mPopulationDeath[role] % divisor;

	mPopulation[role] -= deaths;
	mDeathCount += deaths;

	if (mPopulation[role] == 0)
	{
		mPopulationDeath[role] = 0;
	}
}


void Population::killPopulation(int morale, int nurseries, int hospitals)
{
	const auto mortalityRate = moraleModifierTable[moraleIndex(morale)].mortalityRate;

	int divisorChild = mortalityRate + (nurseries * 10);
	int divisorStudent = mortalityRate + (hospitals * 65);
	int divisorAdult = mortalityRate + 250 + (hospitals * 60);

	// Workers will die more often than scientists.
	auto employableRoleToKill = randomNumber.generate(0, 100) <= 45 ?
		PopulationTable::Role::Scientist : PopulationTable::Role::Worker;

	killRole(PopulationTable::Role::Child, divisorChild);
	killRole(PopulationTable::Role::Student, divisorStudent);
	killRole(employableRoleToKill, divisorAdult);
	killRole(PopulationTable::Role::Retired, divisorAdult);

	if (mPopulation.child <= 0)
	{
		mPopulationGrowth.student = 0;
	}
	if (mPopulation.student <= 0)
	{
		mPopulationGrowth.worker = 0;
	}
}


/**
 * Determine how much food should be consumed and kill off any population that
 * starves.
 *
 * \return	Actual amount of food consumed.
 */
int Population::consumeFood(int food)
{
	// If there's no food kill everybody (humans can survive up to 21 days without food, one turn == minimum 28 days)
	if (food == 0)
	{
		mDeathCount = mPopulation.size();
		clear();
		return 0;
	}

	int PopulationFed = food * 10;
	if (PopulationFed > mPopulation.size())
	{
		return mPopulation.size() / 10;
	}


	int populationToKill = static_cast<int>((mPopulation.size() - PopulationFed) * mStarveRate);
	if (mPopulation.size() == 1) { populationToKill = 1; }

	for (int i = 0; i < populationToKill; /**/)
	{
		std::size_t roleIndex = i % 5;

		std::size_t counter = 0;
		for (;;)
		{
			roleIndex = roleIndex + counter;
			if (roleIndex > 4) { roleIndex = 0; }

			if (mPopulation[roleIndex] > 0)
			{
				break;
			}

			++counter;
			if (counter > 4) { counter = 0; }
		}

		--mPopulation[roleIndex];
		++i;
	}

	mDeathCount = populationToKill;

	// actual amount of food used for the fed part of the population.
	return PopulationFed / 10;
}


/**
 * \return	Actual amount of food consumed.
 */
int Population::update(int morale, int food, int residences, int universities, int nurseries, int hospitals)
{
	mBirthCount = 0;
	mDeathCount = 0;

	spawnChildren(morale, residences, nurseries);
	spawnStudents();
	spawnAdults(universities);
	spawnRetiree();

	killPopulation(morale, nurseries, hospitals);

	return consumeFood(food);
}
