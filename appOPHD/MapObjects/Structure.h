#pragma once

#include <libOPHD/StorableResources.h>
#include <libOPHD/MapObjects/MapObject.h>
#include <libOPHD/Population/PopulationRequirements.h>


namespace NAS2D
{
	class Dictionary;
}


enum class StructureID;
enum class ConnectorDir;
enum class DisabledReason;
enum class IdleReason;
enum class StructureClass;
enum class StructureState;
struct StructureType;
struct MapCoordinate;
class Tile;
class StringTable;


class Structure : public MapObject
{
public:
	Structure(StructureID id, Tile& tile);
	Structure(StructureID id, Tile& tile, const std::string& initialAction);

	~Structure() override = default;

	const StructureType& type() const;
	const std::string& name() const override;

	// STATES & STATE MANAGEMENT
	StructureState state() const { return mStructureState; }

	StructureID structureId() const { return mStructureId; }

	Tile& tile() const;
	const MapCoordinate& xyz() const;

	bool connected() const { return mConnected; }
	void connected(bool value) { mConnected = value; }

	bool disabled() const;
	void disable(DisabledReason);
	DisabledReason disabledReason() const { return mDisabledReason; }

	bool operational() const;
	void enable();

	bool isIdle() const;
	void idle(IdleReason);
	IdleReason idleReason() const { return mIdleReason; }

	bool destroyed() const;
	void destroy();

	bool underConstruction() const;

	void forceIdle(bool forcedIdle);
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
	StructureClass structureClass() const;
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
	int energyProduced() const;
	int foodProduced() const;
	int foodStorageCapacity() const;
	int refinedOreStorageCapacity() const;
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
	bool selfSustained() const;
	bool repairable() const;

	// CONVENIENCE FUCNTIONS
	bool isChap() const;
	bool isCommand() const;
	bool isFactory() const;
	bool isWarehouse() const;
	bool isRobotCommand() const;
	bool isMineFacility() const;
	bool isSmelter() const;
	bool isEnergyProducer() const;
	bool isFoodProducer() const;
	bool isFoodStore() const;
	bool isOreStore() const;
	bool isComms() const;
	bool isPolice() const;
	bool isLander() const;
	bool isPark() const;
	bool isMaintenance() const;
	bool isConnector() const;
	bool isRoad() const;

	void age(int newAge) { mAge = newAge; }
	void connectorDirection(ConnectorDir dir) { mConnectorDirection = dir; }

	virtual void forcedStateChange(StructureState, DisabledReason, IdleReason);

	void rebuild();

	virtual void processTurn();
	virtual void think() {}

	/**
	* Pass limited structure specific details for drawing. Use a custom UI window if needed.
	*/
	virtual StringTable createInspectorViewTable() const;

	virtual NAS2D::Dictionary getDataDict() const;

protected:
	void activate();

	virtual void disabledStateSet() {}

	void state(StructureState newState) { mStructureState = newState; }

	int calculateMaxEnergyProduction() const;

private:
	Structure() = delete;

	void incrementAge();
	void updateIntegrityDecay();

	/**
	 * Provided so that structures that need to do something upon
	 * activation can do so without overriding void activate();
	 */
	virtual void activated() {}

protected:
	const StructureType& mStructureType;
	const StructureID mStructureId;
	const StructureClass mStructureClass;
	Tile& mTile;

	int mAge{0};
	int mCrimeRate{0};
	int mIntegrity{100};

	StructureState mStructureState;
	ConnectorDir mConnectorDirection;

	PopulationRequirements mPopulationAvailable{}; /**< Determine how many of each type of population required was actually supplied to the structure. */

	StorableResources mProductionPool; /**< Resource pool used for production. */
	StorableResources mStoragePool; /**< Resource storage pool. */

	DisabledReason mDisabledReason;
	IdleReason mIdleReason;

	bool mConnected{false};
	bool mForcedIdle{false}; /**< Indicates that the Structure was manually set to Idle by the user and should remain that way until the user says otherwise. */
};


using StructureList = std::vector<Structure*>;

std::vector<StructureClass> allStructureClasses();
