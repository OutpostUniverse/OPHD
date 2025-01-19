#include "CrimeExecution.h"

#include "../Resources.h"
#include "../EnumDifficulty.h"
#include "../StructureManager.h"
#include "../MapObjects/Structures/FoodProduction.h"

#include <libOPHD/RandomNumberGenerator.h>

#include <NAS2D/StringUtils.h>
#include <NAS2D/Utility.h>


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


	int calcAmountForStealing(Difficulty difficulty, int low, int high, int max)
	{
		auto stealRandom = randomNumber.generate(low, high);
		auto stealModified = stealRandom * stealingMultipliers.at(difficulty) / stealingScale;
		auto stealClipped = std::min(stealModified, max);
		return stealClipped;
	}
}


CrimeExecution::CrimeExecution(const Difficulty& difficulty) :
	mDifficulty{difficulty}
{
}


CrimeExecution::CrimeExecution(const Difficulty& difficulty, Signal::DelegateType onCrimeEvent) : CrimeExecution{difficulty}
{
	mCrimeEventSignal.connect(onCrimeEvent);
}


void CrimeExecution::executeCrimes(const std::vector<Structure*>& structuresCommittingCrime)
{
	mMoraleChanges.clear();

	for (auto& structure : structuresCommittingCrime)
	{
		switch (structure->structureId())
		{
		case StructureID::SID_AGRIDOME:
			stealFood(static_cast<FoodProduction&>(*structure));
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

		mCrimeEventSignal.emit(
			"Food Stolen",
			NAS2D::stringFrom(foodStolen) + " units of food was pilfered from a " + structure.name() + ". " + getReasonForStealing() + ".",
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
	if (structure.storage().isEmpty())
	{
		return;
	}

	auto resourceIndicesWithStock = structure.storage().getIndicesWithStock();
	auto indexToStealFrom = randomNumber.generate<std::size_t>(0, resourceIndicesWithStock.size() - 1);

	int amountStolen = calcAmountForStealing(mDifficulty, 2, 5, structure.storage().resources[indexToStealFrom]);
	structure.storage().resources[indexToStealFrom] -= amountStolen;

	mCrimeEventSignal.emit(
		"Resources Stolen",
		NAS2D::stringFrom(amountStolen) + " units of " + resourceNames[indexToStealFrom] + " were stolen from a " + structure.name() + ". " + getReasonForStealing() + ".",
		structure
	);
}


void CrimeExecution::vandalize(Structure& structure)
{
	mMoraleChanges.push_back(std::make_pair("Vandalism", -1));

	mCrimeEventSignal.emit(
		"Vandalism",
		"A " + structure.name() + " was vandalized.",
		structure
	);
}
