#include "CrimeExecution.h"

#include "../StructureManager.h"
#include "../RandomNumberGenerator.h"
#include "../UI/NotificationArea.h"

#include <NAS2D/StringUtils.h>
#include <NAS2D/Utility.h>


CrimeExecution::CrimeExecution(NotificationArea& notificationArea) : mNotificationArea(notificationArea) {}


void CrimeExecution::executeCrimes(const std::vector<Structure*>& structuresCommittingCrime)
{
	mMoraleChanges.clear();

	for (auto& structure : structuresCommittingCrime)
	{
		if (structure == nullptr)
		{
			continue;
		}

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
		int foodStolen = calcAmountForStealing(5, 15);
		if (foodStolen > structure.foodLevel())
		{
			foodStolen = structure.foodLevel();
		}

		structure.foodLevel(-foodStolen);

		const auto& structureTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(&structure);

		mNotificationArea.push({
			"Food Stolen",
			NAS2D::stringFrom(foodStolen) + " units of food was pilfered from a " + structure.name() + ". " + getReasonForStealing() + ".",
			structureTile.xyz(),
			NotificationArea::NotificationType::Warning});
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

	int amountStolen = calcAmountForStealing(2, 5);
	if (amountStolen > structure.storage().resources[indexToStealFrom])
	{
		amountStolen = structure.storage().resources[indexToStealFrom];
	}

	structure.storage().resources[indexToStealFrom] -= amountStolen;

	const auto& structureTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(&structure);

	mNotificationArea.push({
		"Resources Stolen",
		NAS2D::stringFrom(amountStolen) + " units of " + resourceNames[indexToStealFrom] + " were stolen from a " + structure.name() + ". " + getReasonForStealing() + ".",
		structureTile.xyz(),
		NotificationArea::NotificationType::Warning});
}


void CrimeExecution::vandalize(Structure& structure)
{
	mMoraleChanges.push_back(std::make_pair("Vandalism", -1));

	const auto& structureTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(&structure);

	mNotificationArea.push({
		"Vandalism",
		"A " + structure.name() + " was vandalized.",
		structureTile.xyz(),
		NotificationArea::NotificationType::Warning});
}


int CrimeExecution::calcAmountForStealing(int unadjustedMin, int unadjustedMax)
{
	auto amountToSteal = randomNumber.generate(unadjustedMin, unadjustedMax);

	return static_cast<int>(stealingMultipliers.at(mDifficulty) * amountToSteal);
}


std::string CrimeExecution::getReasonForStealing()
{
	return stealingResoureReasons[randomNumber.generate<std::size_t>(0, stealingResoureReasons.size() - 1)];
}
