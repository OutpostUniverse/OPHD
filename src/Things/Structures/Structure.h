#pragma once

#include "../Thing.h"

#include "../../Common.h"
#include "../../PopulationPool.h"
#include "../../ResourcePool.h"

class Structure: public Thing
{
public:
	/**
	 * State of an individual Structure.
	 */
	enum StructureState
	{
		UNDER_CONSTRUCTION,
		OPERATIONAL,
		IDLE,
		DISABLED,
		DESTROYED
	};

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
	enum StructureClass
	{
		CLASS_COMMAND,
		CLASS_COMM,
		CLASS_COMMERCIAL,
		CLASS_ENERGY_PRODUCTION,
		CLASS_FACTORY,
		CLASS_FOOD_PRODUCTION,
		CLASS_LABORATORY,
		CLASS_LANDER,
		CLASS_LIFE_SUPPORT,
		CLASS_MINE,
		CLASS_MEDICAL_CENTER,
		CLASS_NURSERY,
		CLASS_PARK,
		CLASS_SURFACE_POLICE,
		CLASS_UNDERGROUND_POLICE,
		CLASS_RECREATION_CENTER,
		CLASS_RECYCLING,
		CLASS_RESIDENCE,
		CLASS_ROBOT_COMMAND,
		CLASS_SMELTER,
		CLASS_STORAGE,
		CLASS_TUBE,
		CLASS_UNDEFINED,			/**< Used for structures that have no need for classification. */
		CLASS_UNIVERSITY,
		CLASS_WAREHOUSE
	};

public:
	Structure(const std::string& name, const std::string& spritePath, StructureClass structureClass);

	// STATES & STATE MANAGEMENT
	StructureState state() const { return mStructureState; }

	bool disabled() const { return mStructureState == DISABLED; }
	void disable(DisabledReason);
	DisabledReason disabledReason() const { return mDisabledReason; }

	bool operational() const { return mStructureState == OPERATIONAL; }
	void enable();

	bool isIdle() const { return mStructureState == IDLE; }
	void idle(IdleReason);
	IdleReason idleReason() const { return mIdleReason; }

	bool destroyed() const { return mStructureState == DESTROYED; }
	void destroy();

	bool underConstruction() const { return mStructureState == UNDER_CONSTRUCTION; }

	void forceIdle(bool force);
	bool forceIdle() const { return mForcedIdle; }

	// RESOURCES AND RESOURCE MANAGEMENT
	ResourcePool& resourcesIn() { return mResourcesInput; }
	ResourcePool& resourcesOut() { return mResourcesOutput; }

	ResourcePool& storage() { return mStoragePool; }
	ResourcePool& production() { return mProductionPool; }

	virtual void input(ResourcePool& /*pool*/) {}
	bool enoughResourcesAvailable(ResourcePool& r);

	const PopulationRequirements& populationRequirements() const { return mPopulationRequirements; }
	PopulationRequirements& populationAvailable() { return mPopulationAvailable; }

	// ATTRIBUTES
	StructureClass structureClass() const { return mStructureClass; }
	ConnectorDir connectorDirection() const { return mConnectorDirection; }

	int turnsToBuild() const { return mTurnsToBuild; }
	int age() const { return mAge; }
	int maxAge() const { return mMaxAge; }

	// FLAGS
	bool requiresCHAP() const { return mRequiresCHAP; }
	bool providesCHAP() const { return structureClass() == CLASS_LIFE_SUPPORT; }
	bool selfSustained() const { return mSelfSustained; }
	bool repairable() const { return mRepairable; }

	// CONVENIENCE FUCNTIONS
	bool isFactory() const { return structureClass() == CLASS_FACTORY; }
	bool isWarehouse() const { return structureClass() == CLASS_WAREHOUSE; }
	bool isRobotCommand() const { return structureClass() == CLASS_ROBOT_COMMAND; }
	bool isMineFacility() const { return structureClass() == CLASS_MINE; }
	bool energyProducer() const { return structureClass() == CLASS_ENERGY_PRODUCTION; }
	bool isConnector() const { return structureClass() == CLASS_TUBE; }	/** Indicates that the structure can act as a connector (tube) */

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

protected:
	friend class StructureCatalogue;

	void turnsToBuild(int newTurnsToBuild) { mTurnsToBuild = newTurnsToBuild; }
	void maxAge(int newMaxAge) { mMaxAge = newMaxAge; }
	
	void repairable(bool isRepairable) { mRepairable = isRepairable; }

	virtual void defineResourceInput() {}
	virtual void defineResourceOutput() {}

	void activate();

	virtual void disabledStateSet() {}

	void state(StructureState newState) { mStructureState = newState; }

	void requiresCHAP(bool value) { mRequiresCHAP = value; }
	void selfSustained(bool value) { mSelfSustained = value; }

	void setPopulationRequirements(const PopulationRequirements& pr) { mPopulationRequirements = pr; }

private:
	Structure() = delete;

	void incrementAge();
	void die() final;

	/**
	 * Provided so that structures that need to do something upon
	 * activation can do so without overriding void activate();
	 */
	virtual void activated() {}

private:
	int						mTurnsToBuild = 0;			/**< Number of turns it takes to build the Structure. */
	int						mAge = 0;					/**< Age of the Structure in turns. */
	int						mMaxAge = 0;				/**< Maximum number of turns the Structure can remain in good repair. */

	StructureState			mStructureState = UNDER_CONSTRUCTION;			/**< State the structure is in. */
	StructureClass			mStructureClass;								/**< Indicates the Structure's Type. */
	ConnectorDir			mConnectorDirection = CONNECTOR_INTERSECTION;	/**< Directions available for connections. */

	PopulationRequirements	mPopulationRequirements;	/**< Population requirements for structure operation. */
	PopulationRequirements	mPopulationAvailable;		/**< Determine how many of each type of population required was actually supplied to the structure. */

	ResourcePool			mResourcesInput;			/**< Resources needed to operate the Structure. */
	ResourcePool			mResourcesOutput;			/**< Resources provided by the Structure if operating properly. */

	ResourcePool			mProductionPool;			/**< Resource pool used for production. */
	ResourcePool			mStoragePool;				/**< Resource storage pool. */

	DisabledReason			mDisabledReason = DISABLED_NONE;
	IdleReason				mIdleReason = IDLE_NONE;

	bool					mRepairable = true;			/**< Indicates whether or not the Structure can be repaired. Useful for forcing some Structures to die at the end of their life. */
	bool					mRequiresCHAP = true;		/**< Indicates that the Structure needs to have an active CHAP facility in order to operate. */
	bool					mSelfSustained = false;		/**< Indicates that the Structure is self contained and can operate by itself. */
	bool					mForcedIdle = false;		/**< Indicates that the Structure was manually set to Idle by the user and should remain that way until the user says otherwise. */
};


using StructureList = std::vector<Structure*>;

const std::string& structureStateDescription(Structure::StructureState);
const std::string& structureClassDescription(Structure::StructureClass);
