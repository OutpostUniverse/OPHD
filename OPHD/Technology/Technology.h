#pragma once

#include <memory>
#include <string>
#include <vector>


struct Technology
{
	struct Modifier;
	struct Unlock;

	struct Effect
	{
		enum class Type
		{
			Modifier,
			Unlock
		};

		Effect() = delete;
		virtual ~Effect() = default;

		bool isModifier() const;
		bool isUnlock() const;

		Modifier& toModifier();
		Unlock& toUnlock();

	protected:
		Effect(Type type);
		
		Type type;
	};

	struct Modifier : public Effect
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

		Modifier() = delete;
		Modifier(Modifies modifies, float value);
		~Modifier() override = default;

		Modifies modifies;
		float value;
	};

	struct Unlock : public Effect
	{
		enum class Unlocks
		{
			DisasterPrediction,
			Robot,
			Satellite,
			Structure,
			Vehicle
		};

		Unlock() = delete;
		Unlock(Unlocks unlocks, std::string value);
		~Unlock() override = default;

		Unlocks unlocks;
		std::string value;
	};

	std::string name{""};
	std::string description{""};
	
	int id{0};
	int labType{0};
	int cost{0};

	std::vector<int> requiredTechnologies;

	std::vector<std::unique_ptr<Effect>> effects;
};
