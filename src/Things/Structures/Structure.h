#pragma once

#include "../Thing.h"
#include "../../ResourcePool.h"
#include "../../Common.h"



class Structure: public Thing
{
public:

	enum StructureState
	{
		UNDER_CONSTRUCTION,
		OPERATIONAL,
		IDLE,
		DISABLED,
		DESTROYED
	};

	enum StructurePriority
	{
		PRIORITY_LOW,
		PRIORITY_MEDIUM,
		PRIORITY_HIGH,
		PRIORITY_CC
	};

	enum StructureType
	{
		STRUCTURE_ATMOSPHERE_PRODUCTION,
		STRUCTURE_COMMAND,
		STRUCTURE_COMM,
		STRUCTURE_ENERGY_PRODUCTION,
		STRUCTURE_FACTORY,
		STRUCTURE_FOOD_PRODUCTION,
		STRUCTURE_LABORATORY,
		STRUCTURE_LANDER,
		STRUCTURE_MINE,
		STRUCTURE_RECYCLING,
		STRUCTURE_RESIDENCE,
		STRUCTURE_SMELTER,
		STRUCTURE_STORAGE,
		STRUCTURE_TUBE,
		STRUCTURE_UNDEFINED
	};

	Structure(const std::string& name, const std::string& sprite_path, StructureType _t);
	virtual ~Structure();

	//bool operator< (const Structure& rhs) const { return mStructurePriority > rhs.mStructurePriority; }
	
	// STATES & STATE MANAGEMENT
	StructureState state() const { return mStructureState; }

	bool disabled() const { return mStructureState == DISABLED; }
	void disable();

	bool operational() const { return mStructureState == OPERATIONAL; }
	void enable();

	bool isIdle() const { return mStructureState == IDLE; }
	void idle();

	bool destroyed() const { return mStructureState == DESTROYED; }
	void destroy();

	bool underConstruction() const { return mStructureState == UNDER_CONSTRUCTION; }


	// RESOURCES AND RESOURCE MANAGEMENT
	ResourcePool& resourceValue() { return mResourceValue; }
	ResourcePool& resourcesIn() { return mResourcesInput; }
	ResourcePool& resourcesOut() { return mResourcesOutput; }

	ResourcePool& storage() { return mStoragePool; }

	void input(ResourcePool& _resourcePool);
	bool enoughResourcesAvailable(ResourcePool& r);


	// ATTRIBUTES
	StructurePriority priority() const { return mStructurePriority; }

	StructureType type() const { return mStructureType; }

	int id() const { return mId; }
	int turnsToBuild() const { return mTurnsToBuild; }
	int age() const { return mAge; }
	int maxAge() const { return mMaxAge; }

	bool repairable() const { return mRepairable; }

	bool isConnector() const { return mConnector; } /** Indicates that the structure can act as a connector (tube) */
	ConnectorDir connectorDirection() const { return mConnectorDirection; }

	// FLAGS
	bool requiresCHAP() const { return mRequiresCHAP; }
	bool providesCHAP() const { return mProvidesCHAP; }
	bool selfSustained() const { return mSelfSustained; }
	bool isFactory() const { return mIsFactory; }

	void update();

protected:

	virtual void think() {}

	void turnsToBuild(int _t) { mTurnsToBuild = _t; }
	void maxAge(int _age) { mMaxAge = _age; }
	
	/**
	 * Set the current age of the Structure.
	 * 
	 * \note	Available to reset current age to simulate repairs to extend
	 *			the life of the Structure and for loading games.
	 */
	void age(int _age) { mAge = _age; }

	void repairable(bool _r) { mRepairable = _r; }

	virtual void defineResourceInput() {}
	virtual void defineResourceOutput() {}
	virtual void defineResourceValue() {}

	void state(StructureState _s) { mStructureState = _s; }

	void activate();

	void isConnector(bool _b) { mConnector = _b; }
	void connectorDirection(ConnectorDir _cd) { mConnectorDirection = _cd; }

	void requiresCHAP(bool _b) { mRequiresCHAP = _b; }
	void providesCHAP(bool _b) { mProvidesCHAP = _b; }
	void selfSustained(bool _b) { mSelfSustained = _b; }

	void isFactory(bool _b) { mIsFactory = _b; }

	void priority(StructurePriority _sp) { mStructurePriority = _sp; }
	void type(StructureType _t) { mStructureType = _t; }

	ResourcePool			mResourcesNeededToBuild;	/**< Resource needed to build the Structure */
	
	ResourcePool			mResourcesInput;			/**< Resources needed to operate the Structure. */
	ResourcePool			mResourcesOutput;			/**< Resources provided by the Structure if operating properly. */

	ResourcePool			mStoragePool;				/**< Resource storage pool. */

	ResourcePool			mResourceValue;				/**< Resource reclamation values when dozed. */

private:

	Structure();	// Excplicitly declared private

	void incrementAge();

	int						mId;						/**< ID of the Structure. */

	int						mTurnsToBuild;				/**< Number of turns it takes to build the Structure. */

	int						mAge;						/**< Age of the Structure in turns. */
	int						mMaxAge;					/**< Maximum number of turns the Structure can remain in good repair. */

	StructureState			mStructureState;			/**< State the structure is in. */

	StructurePriority		mStructurePriority;			/**< Indicates the priority level that the Structure gets when processing resources. */

	StructureType			mStructureType;				/**< Indicates the Structure's Type. */

	ConnectorDir			mConnectorDirection;		/**< Directions available for connections. */

	bool					mRepairable;				/**< Indicates whether or not the Structure can be repaired. Useful for forcing some Structures die at the end of their life. */
	bool					mConnector;					/**< Indicates that the Structure can act as a connector (tube) */
	bool					mRequiresCHAP;				/**< Indicates that the Structure needs to have an active CHAP facilities in order to operate. */
	bool					mProvidesCHAP;				/**< Indicates that the Strcuture provides CHAP. */
	bool					mSelfSustained;				/**< Indicates that the Strcutre is self contained and can operate by itself. */
	bool					mIsFactory;					/**< Indicates that the Structure is a factory. */
};
