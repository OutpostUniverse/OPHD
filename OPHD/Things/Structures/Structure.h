#pragma once

#include "../Thing.h"

#include "../../Common.h"
#include "../../PopulationPool.h"
#include "../../StorableResources.h"
#include "../../UI/StringTable.h"

#include <NAS2D/Dictionary.h>


/**
 * State of an individual Structure.
 */
enum class StructureState
{
	UnderConstruction,
	Operational,
	Idle,
	Disabled,
	Destroyed
};

class Structure : public Thing
{
public:
	/**
	 * Class of a Structure.
	 *
	 * Structures are grouped by 'class'. Basically it's just an easy
	 * way to know how to sort structures so that they can be updated
	 * based on priority.
	 *
	 * \note	Some structure classes will only have one structure
	 *			that uses it. This is intended behavior.
	 */
	enum class StructureClass
	{
		Command,
		Communication,
		Commercial,
		EnergyProduction,
		Factory,
		FoodProduction,
		Laboratory,
		Lander,
		LifeSupport,
		Maintenance,
		Mine,
		MedicalCenter,
		Nursery,
		Park,
		Road,
		SurfacePolice,
		UndergroundPolice,
		RecreationCenter,
		Recycling,
		Residence,
		RobotCommand,
		Smelter,
		Storage,
		Tube,
		Undefined, /**< Used for structures that have no need for classification. */
		University,
		Warehouse
	};

public:
	Structure(const std::string& name, const std::string& spritePath, StructureClass structureClass, StructureID id);
	Structure(const std::string& name, const std::string& spritePath, const std::string& initialAction, StructureClass structureClass, StructureID id);

	~Structure() override = default;

	// STATES & STATE MANAGEMENT
	StructureState state() const { return mStructureState; }

	StructureID structureId() const { return mStructureId; }

	bool disabled() const { return mStructureState == StructureState::Disabled; }
	void disable(DisabledReason);
	DisabledReason disabledReason() const { return mDisabledReason; }

	bool operational() const { return mStructureState == StructureState::Operational; }
	void enable();

	bool isIdle() const { return mStructureState == StructureState::Idle; }
	void idle(IdleReason);
	IdleReason idleReason() const { return mIdleReason; }

	bool destroyed() const { return mStructureState == StructureState::Destroyed; }
	void destroy();

	bool underConstruction() const { return mStructureState == StructureState::UnderConstruction; }

	void forceIdle(bool force);
	bool forceIdle() const { return mForcedIdle; }

	// RESOURCES AND RESOURCE MANAGEMENT
	const StorableResources& resourcesIn() const { return mResourcesInput; }

	StorableResources& storage() { return mStoragePool; }
	StorableResources& production() { return mProductionPool; }

	const PopulationRequirements& populationRequirements() const { return mPopulationRequirements; }
	PopulationRequirements& populationAvailable() { return mPopulationAvailable; }

	// ATTRIBUTES
	StructureClass structureClass() const { return mStructureClass; }
	const std::string& stateDescription() const;
	static const std::string& stateDescription(StructureState state);
	const std::string& classDescription() const;
	static const std::string& classDescription(Structure::StructureClass structureClass);
	ConnectorDir connectorDirection() const { return mConnectorDirection; }

	int turnsToBuild() const { return mTurnsToBuild; }
	int age() const { return mAge; }
	int maxAge() const { return mMaxAge; }
	bool ages() const { return maxAge() > 0; }

	int energyRequirement() const { return mEnergyRequirement; }
	int storageCapacity() const { return mStorageCapacity; }

	bool hasCrime() const { return mHasCrime; }
	int crimeRate() const { return mCrimeRate; }
	void crimeRate(int crimeRate);
	void increaseCrimeRate(int deltaCrimeRate);

	int integrity() const { return mIntegrity; }
	void integrity(int integrity);
	int integrityDecayRate() const { return mIntegrityDecayRate; }

	// FLAGS
	bool requiresCHAP() const { return mRequiresCHAP; }
	bool providesCHAP() const { return mStructureClass == StructureClass::LifeSupport; }
	bool selfSustained() const { return mSelfSustained; }
	bool repairable() const { return mRepairable; }

	// CONVENIENCE FUCNTIONS
	bool isFactory() const { return mStructureClass == StructureClass::Factory; }
	bool isWarehouse() const { return mStructureClass == StructureClass::Warehouse; }
	bool isRobotCommand() const { return mStructureClass == StructureClass::RobotCommand; }
	bool isMineFacility() const { return mStructureClass == StructureClass::Mine; }
	bool isEnergyProducer() const { return mStructureClass == StructureClass::EnergyProduction; }
	bool isConnector() const { return mStructureClass == StructureClass::Tube; }
	bool isRoad() const { return mStructureClass == StructureClass::Road; }

	void age(int newAge) { mAge = newAge; }
	void connectorDirection(ConnectorDir dir) { mConnectorDirection = dir; }

	virtual void forced_state_change(StructureState, DisabledReason, IdleReason);

	void rebuild(size_t turnsToRebuild);

	void update() override;
	virtual void think() {}

	/**
	* Pass limited structure specific details for drawing. Use a custom UI window if needed.
	*/
	virtual StringTable createInspectorViewTable() { return StringTable(0, 0); }

	virtual NAS2D::Dictionary getDataDict() const;

protected:
	friend class StructureCatalogue;

	void turnsToBuild(int newTurnsToBuild) { mTurnsToBuild = newTurnsToBuild; }
	void maxAge(int newMaxAge) { mMaxAge = newMaxAge; }

	void integrityDecayRate(int decayRate) { mIntegrityDecayRate = decayRate; }

	void repairable(bool isRepairable) { mRepairable = isRepairable; }

	virtual void defineResourceInput() {}

	void activate();

	virtual void disabledStateSet() {}

	void state(StructureState newState) { mStructureState = newState; }

	void requiresCHAP(bool value) { mRequiresCHAP = value; }
	void selfSustained(bool value) { mSelfSustained = value; }
	void hasCrime(bool value) { mHasCrime = value; }

	void setPopulationRequirements(const PopulationRequirements& pr) { mPopulationRequirements = pr; }
	void energyRequired(int energy) { mEnergyRequirement = energy; }

	void resourcesIn(const StorableResources& resources) { mResourcesInput = resources; }

	void storageCapacity(int capacity) { mStorageCapacity = capacity; }

private:
	Structure() = delete;

	void incrementAge();
	void updateIntegrityDecay();
	void die() override;

	/**
	 * Provided so that structures that need to do something upon
	 * activation can do so without overriding void activate();
	 */
	virtual void activated() {}

private:
	int mTurnsToBuild{0};
	int mAge{0};
	int mMaxAge{0};
	int mEnergyRequirement{0};
	int mStorageCapacity{0};
	int mCrimeRate{0};
	int mIntegrity{100};
	int mIntegrityDecayRate{1};

	StructureID mStructureId{StructureID::SID_NONE};

	StructureState mStructureState{StructureState::UnderConstruction};
	StructureClass mStructureClass{StructureClass::Undefined};
	ConnectorDir mConnectorDirection{ConnectorDir::CONNECTOR_INTERSECTION};

	PopulationRequirements mPopulationRequirements{}; /**< Population requirements for structure operation. */
	PopulationRequirements mPopulationAvailable{}; /**< Determine how many of each type of population required was actually supplied to the structure. */

	StorableResources mResourcesInput; /**< Resources needed to operate the Structure. */

	StorableResources mProductionPool; /**< Resource pool used for production. */
	StorableResources mStoragePool; /**< Resource storage pool. */

	DisabledReason mDisabledReason{DisabledReason::None};
	IdleReason mIdleReason{IdleReason::None};

	bool mRepairable{true};
	bool mRequiresCHAP{true};
	bool mSelfSustained{false};
	bool mHasCrime{false};
	bool mForcedIdle{false}; /**< Indicates that the Structure was manually set to Idle by the user and should remain that way until the user says otherwise. */
};


using StructureList = std::vector<Structure*>;

std::string StructureName(StructureID id);
