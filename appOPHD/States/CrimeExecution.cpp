#include "CrimeExecution.h"

#include "../Resources.h"
#include "../MapObjects/Structure.h"
#include "../MapObjects/Structures/FoodProduction.h"

#include <libOPHD/EnumDifficulty.h>
#include <libOPHD/RandomNumberGenerator.h>
#include <libOPHD/Population/MoraleChangeEntry.h>

#include <map>


namespace
{
	constexpr int stealingScale = 4;
	const std::map<Difficulty, int> stealingMultipliers
	{
		{Difficulty::Beginner, 2},
		{Difficulty::Easy, 3},
		{Difficulty::Medium, 4},
		{Difficulty::Hard, 6}
	};

	const std::vector<std::string> stealingResoureReasons
	{
		"There are no identified suspects",
		"An investigation has been opened",
		"A local crime syndicate is under investigation",
		"A suspect was apprehended but the goods remain unaccounted for",
		"A separatist political movement has claimed responsibility",
		"The rebel faction is suspected in preparation for a splinter colony"
	};


	std::string getReasonForStealing()
	{
		return stealingResoureReasons[randomNumber.generate<std::size_t>(0, stealingResoureReasons.size() - 1)];
	}


	std::vector<std::size_t> getIndicesWithStock(const StorableResources& storabeResources)
	{
		std::vector<std::size_t> indicesWithStock;

		for (std::size_t i = 0; i < storabeResources.resources.size(); ++i)
		{
			if (storabeResources.resources[i] > 0)
			{
				indicesWithStock.push_back(i);
			}
		}

		return indicesWithStock;
	}


	int calcAmountForStealing(Difficulty difficulty, int low, int high, int max)
	{
		auto stealRandom = randomNumber.generate(low, high);
		auto stealModified = stealRandom * stealingMultipliers.at(difficulty) / stealingScale;
		auto stealClipped = std::min(stealModified, max);
		return stealClipped;
	}
}


CrimeExecution::CrimeExecution(const Difficulty& difficulty, CrimeEventDelegate crimeEventHandler) :
	mDifficulty{difficulty},
	mCrimeEventHandler{crimeEventHandler}
{
	if (mCrimeEventHandler.empty())
	{
		throw std::runtime_error("CrimeExecution needs a non-empty crimeEventHandler");
	}
}


std::vector<MoraleChangeEntry> CrimeExecution::moraleChanges() const
{
	return mMoraleChanges;
}


void CrimeExecution::executeCrimes(const std::vector<Structure*>& structuresCommittingCrime)
{
	mMoraleChanges.clear();

	for (auto& structure : structuresCommittingCrime)
	{
		switch (structure->structureId())
		{
		case StructureID::SID_AGRIDOME:
			stealFood(dynamic_cast<FoodProduction&>(*structure));
			break;
		case StructureID::SID_SMELTER:
			stealRawResources(*structure);
			break;
		case StructureID::SID_STORAGE_TANKS:
			stealRefinedResources(*structure);
			break;
		case StructureID::SID_PARK:
			vandalize(*structure);
			break;
		default:
			break;
		}
	}
}


void CrimeExecution::stealFood(FoodProduction& structure)
{
	if (structure.foodLevel() > 0)
	{
		int foodStolen = calcAmountForStealing(mDifficulty, 5, 15, structure.foodLevel());
		structure.foodLevel(structure.foodLevel() - foodStolen);

		mCrimeEventHandler(
			"Food Stolen",
			std::to_string(foodStolen) + " units of food was pilfered from a " + structure.name() + ". " + getReasonForStealing() + ".",
			structure
		);
	}
}


void CrimeExecution::stealRefinedResources(Structure& structure)
{
	stealResources(structure, ResourceNamesRefined);
}


void CrimeExecution::stealRawResources(Structure& structure)
{
	stealResources(structure, ResourceNamesOre);
}


void CrimeExecution::stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames)
{
	auto& storage = structure.storage();
	if (storage.isEmpty())
	{
		return;
	}

	auto resourceIndicesWithStock = getIndicesWithStock(storage);
	auto indexToStealFrom = randomNumber.generate<std::size_t>(0, resourceIndicesWithStock.size() - 1);

	int amountStolen = calcAmountForStealing(mDifficulty, 2, 5, storage.resources[indexToStealFrom]);
	storage.resources[indexToStealFrom] -= amountStolen;

	mCrimeEventHandler(
		"Resources Stolen",
		std::to_string(amountStolen) + " units of " + resourceNames[indexToStealFrom] + " were stolen from a " + structure.name() + ". " + getReasonForStealing() + ".",
		structure
	);
}


void CrimeExecution::vandalize(Structure& structure)
{
	mMoraleChanges.push_back({"Vandalism", -1});

	mCrimeEventHandler(
		"Vandalism",
		"A " + structure.name() + " was vandalized.",
		structure
	);
}
