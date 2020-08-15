#pragma once

#include "../Thing.h"

#include "../../Common.h"
#include "../../PopulationPool.h"
#include "../../StorableResources.h"
#include "../../UI/StringTable.h"

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

class Structure: public Thing
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
	ConnectorDir connectorDirection() const { return mConnectorDirection; }

	int turnsToBuild() const { return mTurnsToBuild; }
	int age() const { return mAge; }
	int maxAge() const { return mMaxAge; }
	int energyRequirement() const { return mEnergyRequirement; }

	// FLAGS
	bool requiresCHAP() const { return mRequiresCHAP; }
	bool providesCHAP() const { return structureClass() == StructureClass::LifeSupport; }
	bool selfSustained() const { return mSelfSustained; }
	bool repairable() const { return mRepairable; }

	// CONVENIENCE FUCNTIONS
	bool isFactory() const { return structureClass() == StructureClass::Factory; }
	bool isWarehouse() const { return structureClass() == StructureClass::Warehouse; }
	bool isRobotCommand() const { return structureClass() == StructureClass::RobotCommand; }
	bool isMineFacility() const { return structureClass() == StructureClass::Mine; }
	bool energyProducer() const { return structureClass() == StructureClass::EnergyProduction; }
	bool isConnector() const { return structureClass() == StructureClass::Tube; } /** Indicates that the structure can act as a connector (tube) */
	bool isRoad() const { return structureClass() == StructureClass::Road; }

	/**
	 * Set the current age of the Structure.
	 * 
	 * \note	Available to reset current age to simulate repairs to extend
	 *			the life of the Structure and for loading games.
	 */
	void age(int newAge) { mAge = newAge; }
	void connectorDirection(ConnectorDir dir) { mConnectorDirection = dir; }

	virtual void forced_state_change(StructureState, DisabledReason, IdleReason);

	void update() override;
	virtual void think() {}

	/**
	* Pass limited structure specific details for drawing. Use a custom UI window if needed.
	*/
	virtual StringTable createInspectorViewTable() { return StringTable(0, 0); }

protected:
	friend class StructureCatalogue;

	void turnsToBuild(int newTurnsToBuild) { mTurnsToBuild = newTurnsToBuild; }
	void maxAge(int newMaxAge) { mMaxAge = newMaxAge; }
	
	void repairable(bool isRepairable) { mRepairable = isRepairable; }

	virtual void defineResourceInput() {}

	void activate();

	virtual void disabledStateSet() {}

	void state(StructureState newState) { mStructureState = newState; }

	void requiresCHAP(bool value) { mRequiresCHAP = value; }
	void selfSustained(bool value) { mSelfSustained = value; }

	void setPopulationRequirements(const PopulationRequirements& pr) { mPopulationRequirements = pr; }
	void energyRequired(int energy) { mEnergyRequirement = energy; }

	void resourcesIn(const StorableResources& resources) { mResourcesInput = resources; }

private:
	Structure() = delete;

	void incrementAge();
	void die() override;

	/**
	 * Provided so that structures that need to do something upon
	 * activation can do so without overriding void activate();
	 */
	virtual void activated() {}

private:
	int mTurnsToBuild = 0; /**< Number of turns it takes to build the Structure. */
	int mAge = 0; /**< Age of the Structure in turns. */
	int mMaxAge = 0; /**< Maximum number of turns the Structure can remain in good repair. */
	int mEnergyRequirement = 0;

	StructureID mStructureId{ StructureID::SID_NONE };

	StructureState mStructureState = StructureState::UnderConstruction; /**< State the structure is in. */
	StructureClass mStructureClass; /**< Indicates the Structure's Type. */
	ConnectorDir mConnectorDirection = ConnectorDir::CONNECTOR_INTERSECTION; /**< Directions available for connections. */

	PopulationRequirements mPopulationRequirements; /**< Population requirements for structure operation. */
	PopulationRequirements mPopulationAvailable; /**< Determine how many of each type of population required was actually supplied to the structure. */

	StorableResources mResourcesInput; /**< Resources needed to operate the Structure. */

	StorableResources mProductionPool; /**< Resource pool used for production. */
	StorableResources mStoragePool; /**< Resource storage pool. */

	DisabledReason mDisabledReason = DisabledReason::None;
	IdleReason mIdleReason = IdleReason::None;

	bool mRepairable = true; /**< Indicates whether or not the Structure can be repaired. Useful for forcing some Structures to die at the end of their life. */
	bool mRequiresCHAP = true; /**< Indicates that the Structure needs to have an active CHAP facility in order to operate. */
	bool mSelfSustained = false; /**< Indicates that the Structure is self contained and can operate by itself. */
	bool mForcedIdle = false; /**< Indicates that the Structure was manually set to Idle by the user and should remain that way until the user says otherwise. */
};


using StructureList = std::vector<Structure*>;

const std::string& structureStateDescription(StructureState);
const std::string& structureClassDescription(Structure::StructureClass);
