#pragma once

#include <memory>
#include <string>
#include <vector>


struct Technology
{
	struct Modifier
	{
		enum class Modifies
		{
			AgricultureEfficiency,
			BreakdownRate,
			EducationEfficiency,
			MaintenanceCost,
			PopulationFertility,
			PopulationMorale,
			PopulationMortality,
			RecyclingEfficiency,
			SmelterEfficiency,
			StructureCost,
			StructureDecay
		};

		Modifies modifies;
		float value;
	};


	struct Unlock
	{
		enum class Unlocks
		{
			DisasterPrediction,
			Robot,
			Satellite,
			Structure,
			Vehicle
		};

		Unlocks unlocks;
		std::string value;
	};


	std::string name{""};
	std::string description{""};
	
	int id{};
	int labType{};
	int cost{};
	int iconIndex{};

	std::vector<int> requiredTechnologies;

	std::vector<Modifier> modifiers;
	std::vector<Unlock> unlocks;
};
