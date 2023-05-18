#pragma once

#include "MapObject.h"

#include "../Common.h"
#include "../PopulationPool.h"
#include "../StorableResources.h"
#include "../UI/StringTable.h"

#include <NAS2D/Dictionary.h>


struct StructureType;


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

class Structure : public MapObject
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
	Structure(StructureClass structureClass, StructureID id);
	Structure(const std::string& initialAction, StructureClass structureClass, StructureID id);

	~Structure() override = default;

	// STATES & STATE MANAGEMENT
	StructureState state() const { return mStructureState; }

	StructureID structureId() const { return mStructureId; }

	bool connected() const { return mConnected; }
	void connected(bool value) { mConnected = value; }

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
	const StorableResources& resourcesIn() const;

	StorableResources& storage() { return mStoragePool; }
	StorableResources& production() { return mProductionPool; }

	const PopulationRequirements& populationRequirements() const;
	PopulationRequirements& populationAvailable() { return mPopulationAvailable; }

	// ATTRIBUTES
	StructureClass structureClass() const { return mStructureClass; }
	const std::string& stateDescription() const;
	static const std::string& stateDescription(StructureState state);
	const std::string& classDescription() const;
	static const std::string& classDescription(StructureClass structureClass);
	ConnectorDir connectorDirection() const { return mConnectorDirection; }

	int turnsToBuild() const;
	int age() const { return mAge; }
	int maxAge() const;
	bool ages() const { return maxAge() > 0; }

	int energyRequirement() const;
	int storageCapacity() const;

	bool hasCrime() const;
	int crimeRate() const { return mCrimeRate; }
	void crimeRate(int crimeRate);
	void increaseCrimeRate(int deltaCrimeRate);

	int integrity() const { return mIntegrity; }
	void integrity(int integrity);
	int integrityDecayRate() const;

	// FLAGS
	bool requiresCHAP() const;
	bool providesCHAP() const { return mStructureClass == StructureClass::LifeSupport; }
	bool selfSustained() const;
	bool repairable() const;

	// CONVENIENCE FUCNTIONS
	bool isFactory() const { return mStructureClass == StructureClass::Factory; }
	bool isWarehouse() const { return mStructureClass == StructureClass::Warehouse; }
	bool isRobotCommand() const { return mStructureClass == StructureClass::RobotCommand; }
	bool isMineFacility() const { return mStructureClass == StructureClass::Mine; }
	bool isSmelter() const { return mStructureClass == StructureClass::Smelter; }
	bool isEnergyProducer() const { return mStructureClass == StructureClass::EnergyProduction; }
	bool isFoodStore() const { return mStructureClass == StructureClass::FoodProduction || mStructureId == StructureID::SID_COMMAND_CENTER; }
	bool isPolice() const { return mStructureClass == StructureClass::SurfacePolice || mStructureClass == StructureClass::UndergroundPolice; }
	bool isLander() const { return mStructureClass == StructureClass::Lander; }
	bool isConnector() const { return mStructureClass == StructureClass::Tube; }
	bool isRoad() const { return mStructureClass == StructureClass::Road; }

	void age(int newAge) { mAge = newAge; }
	void connectorDirection(ConnectorDir dir) { mConnectorDirection = dir; }

	virtual void forced_state_change(StructureState, DisabledReason, IdleReason);

	void rebuild();

	void update() override;
	virtual void think() {}

	/**
	* Pass limited structure specific details for drawing. Use a custom UI window if needed.
	*/
	virtual StringTable createInspectorViewTable() { return StringTable(0, 0); }

	virtual NAS2D::Dictionary getDataDict() const;

protected:
	friend class StructureCatalogue;

	void activate();

	virtual void disabledStateSet() {}

	void state(StructureState newState) { mStructureState = newState; }

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
	const StructureType& mStructureType;

	StructureID mStructureId{StructureID::SID_NONE};

	int mAge{0};
	int mCrimeRate{0};
	int mIntegrity{100};

	StructureState mStructureState{StructureState::UnderConstruction};
	StructureClass mStructureClass{StructureClass::Undefined};
	ConnectorDir mConnectorDirection{ConnectorDir::CONNECTOR_INTERSECTION};

	PopulationRequirements mPopulationAvailable{}; /**< Determine how many of each type of population required was actually supplied to the structure. */

	StorableResources mProductionPool; /**< Resource pool used for production. */
	StorableResources mStoragePool; /**< Resource storage pool. */

	DisabledReason mDisabledReason{DisabledReason::None};
	IdleReason mIdleReason{IdleReason::None};

	bool mConnected{false};
	bool mForcedIdle{false}; /**< Indicates that the Structure was manually set to Idle by the user and should remain that way until the user says otherwise. */
};


using StructureList = std::vector<Structure*>;

std::string StructureName(StructureID id);
std::vector<Structure::StructureClass> allStructureClasses();
