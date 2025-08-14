#pragma once

#include "../StorableResources.h"
#include "../Population/PopulationRequirements.h"

#include <string>


struct StructureType
{
	std::string name;
	std::string description;
	std::string spritePath;

	StorableResources buildCost{};
	StorableResources operationalCost{};
	PopulationRequirements populationRequirements{};

	int turnsToBuild{0};
	int maxAge{0};
	int priority{0};
	int energyRequired{0};
	int energyProduced{0};
	int solarEnergyProduced{0};
	int foodProduced{0};
	int foodStorageCapacity{0};
	int residentialCapacity{0};
	int rawOreStorageCapacity{0};
	int oreStorageCapacity{0};
	int robotCommandCapacity{0};
	int commRange{0};
	int policeRange{0};
	int integrityDecayRate{1};

	bool isSelfSustained{false};
	bool isRepairable{true};
	bool isChapRequired{true};
	bool isCrimeTarget{false};
};
