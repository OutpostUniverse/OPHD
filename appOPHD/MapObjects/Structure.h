#pragma once

#include "StructureClass.h"

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumDisabledReason.h>
#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/EnumStructureID.h>
#include <libOPHD/StorableResources.h>
#include <libOPHD/MapObjects/MapObject.h>
#include <libOPHD/Population/PopulationPool.h>


namespace NAS2D
{
	class Dictionary;
}


struct StructureType;
class StringTable;


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
	Structure(StructureClass structureClass, StructureID id);
	Structure(const std::string& initialAction, StructureClass structureClass, StructureID id);

	~Structure() override = default;

	const std::string& name() const override;

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

	const StorableResources& storage() const { return mStoragePool; }
	const StorableResources& production() const { return mProductionPool; }

	const PopulationRequirements& populationRequirements() const;
	const PopulationRequirements& populationAvailable() const { return mPopulationAvailable; }
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
	int foodProduced() const;
	int foodStorageCapacity() const;
	int storageCapacity() const;
	int commRange() const;
	int policeRange() const;

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
	bool isFactory() const;
	bool isWarehouse() const;
	bool isRobotCommand() const;
	bool isMineFacility() const;
	bool isSmelter() const;
	bool isEnergyProducer() const;
	bool isFoodStore() const;
	bool isPolice() const;
	bool isLander() const;
	bool isConnector() const;
	bool isRoad() const;

	void age(int newAge) { mAge = newAge; }
	void connectorDirection(ConnectorDir dir) { mConnectorDirection = dir; }

	virtual void forced_state_change(StructureState, DisabledReason, IdleReason);

	void rebuild();

	virtual void processTurn();
	virtual void think() {}

	/**
	* Pass limited structure specific details for drawing. Use a custom UI window if needed.
	*/
	virtual StringTable createInspectorViewTable();

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

	/**
	 * Provided so that structures that need to do something upon
	 * activation can do so without overriding void activate();
	 */
	virtual void activated() {}

private:
	const StructureType& mStructureType;
	const StructureID mStructureId{StructureID::SID_NONE};

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
std::vector<StructureClass> allStructureClasses();
