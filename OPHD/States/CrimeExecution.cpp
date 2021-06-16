#include "CrimeExecution.h"
#include "../StructureManager.h"
#include <NAS2D/StringUtils.h>
#include <NAS2D/Utility.h>
#include <algorithm>


CrimeExecution::CrimeExecution(NotificationArea& notificationArea) : mNotificationArea(notificationArea) {}


void CrimeExecution::executeCrimes(const std::vector<Structure*>& structuresCommittingCrime)
{
	for (auto& structure : structuresCommittingCrime)
	{
		if (structure == nullptr) 
		{
			continue;
		}

		switch (structure->structureId())
		{
		case StructureID::SID_AGRIDOME:
			executeStealingFood(static_cast<FoodProduction&>(*structure));
			break;
		default:
			break;
		}
	}
}


void CrimeExecution::executeStealingFood(FoodProduction& structure)
{
	if (structure.foodLevel() > 0)
	{
		int foodStolen = std::clamp(100, 0, structure.foodLevel());
		
		structure.foodLevel(-foodStolen);

		const auto& structureTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(&structure);
		
		mNotificationArea.push("Food Stolen", NAS2D::stringFrom(foodStolen) + " units of food was pilfered.",
			structureTile.position(),
			NotificationArea::NotificationType::Warning);
	}
}
