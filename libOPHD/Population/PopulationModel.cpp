#include "PopulationModel.h"
#include "Morale.h"

#include "../RandomNumberGenerator.h"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>


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


const PopulationTable& PopulationModel::getPopulations() const
{
	return mPopulation;
}


void PopulationModel::addPopulation(const PopulationTable& population)
{
	mPopulation += population;
}

void PopulationModel::removePopulation(const PopulationTable& population)
{
	mPopulation -= population.cap(mPopulation);
}


void PopulationModel::spawnPopulation(int morale, int residences, int nurseries, int universities)
{
	const int growthChild = (residences > 0 || nurseries > 0) ?
		mPopulation.scientist / 4 + mPopulation.worker / 2 : 0;

	// Account for universities
	const int convertRate = (universities > 0) ? studentToScientistRate : 0;
	const int growthWorker = mPopulation.student * (100 - convertRate) / 100;
	const int growthScientist = mPopulation.student * convertRate / 100;

	int totalAdults = mPopulation.worker + mPopulation.scientist;

	int divisorChild = moraleModifierTable[moraleIndex(morale)].fertilityRate;
	int divisorStudent = ((std::max(mPopulation.adults(), studentToAdultBase) / 40) * 3 + 16) * 4;
	int divisorAdult = ((std::max(mPopulation.adults(), studentToAdultBase) / 40) * 3 + 45) * 4;
	int divisorRetiree = ((std::max(totalAdults, adultToRetireeBase) / 40) * 3 + 40) * 4;

	const auto newRoles = spawnRoles(
		{growthChild, mPopulation.child, growthWorker, growthScientist, totalAdults / 10},
		{divisorChild, divisorStudent, divisorAdult, divisorAdult, divisorRetiree}
	);

	mBirthCount = newRoles.child;
	mPopulation.child -= newRoles.student;
	mPopulation.student -= (newRoles.worker + newRoles.scientist);

	if (newRoles.retiree > mPopulation.employable())
	{
		throw std::runtime_error("Retiring more people than employable population: Retiring: " + std::to_string(newRoles.retiree));
	}

	for (int toRetire = newRoles.retiree; toRetire > 0;)
	{
		/** Workers retire earlier than scientists. */
		auto& retireRole = randomNumber.generate(0, 100) <= 45 ?
			mPopulation.scientist : mPopulation.worker;
		if (retireRole > 0)
		{
			--retireRole;
			--toRetire;
		}
	}
}


PopulationTable PopulationModel::spawnRoles(const PopulationTable& growth, const PopulationTable& divisor)
{
	mPopulationGrowth += growth;
	const auto newRoles = mPopulationGrowth / divisor;
	mPopulationGrowth = mPopulationGrowth % divisor;
	mPopulation += newRoles;
	return newRoles;
}


void PopulationModel::killRoles(const PopulationTable& divisor)
{
	mPopulationDeath += mPopulation;

	const auto deaths = (mPopulationDeath / divisor).cap(mPopulation);
	mPopulationDeath = mPopulationDeath % divisor;

	mPopulation -= deaths;
	mDeathCount += deaths.size();

	const auto roleCount = sizeof(PopulationTable) / sizeof(int);
	for (std::size_t role = 0; role < roleCount; ++role)
	{
		if (mPopulation[role] == 0)
		{
			mPopulationDeath[role] = 0;
		}
	}
}


void PopulationModel::killPopulation(int morale, int nurseries, int hospitals)
{
	const auto mortalityRate = moraleModifierTable[moraleIndex(morale)].mortalityRate;

	int divisorChild = mortalityRate + (nurseries * 10);
	int divisorStudent = mortalityRate + (hospitals * 65);
	int divisorAdult = mortalityRate + 250 + (hospitals * 60);

	killRoles({divisorChild, divisorStudent, divisorAdult * 2 - 50, divisorAdult * 2 + 50, divisorAdult});

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
int PopulationModel::consumeFood(int food)
{
	const int PopulationPerFood = 10;
	const int populationFed = food * PopulationPerFood;
	const int populationUnfed = mPopulation.size() - populationFed;
	const int minKill = std::clamp(populationUnfed, 0, 1);
	const int populationToKill = std::clamp(static_cast<int>(static_cast<float>(populationUnfed) * mStarveRate), minKill, mPopulation.size());
	mDeathCount += populationToKill;

	for (int i = populationToKill; i > 0; mStarveRoleIndex = (mStarveRoleIndex + 1) % 5)
	{
		if (mPopulation[mStarveRoleIndex] > 0)
		{
			--mPopulation[mStarveRoleIndex];
			--i;
		}
	}

	// Round up food consumption for remaining people
	return (mPopulation.size() + (PopulationPerFood - 1)) / PopulationPerFood;
}


/**
 * \return	Actual amount of food consumed.
 */
int PopulationModel::update(int morale, int food, int residences, int universities, int nurseries, int hospitals)
{
	mBirthCount = 0;
	mDeathCount = 0;

	spawnPopulation(morale, residences, nurseries, universities);
	killPopulation(morale, nurseries, hospitals);

	return consumeFood(food);
}
