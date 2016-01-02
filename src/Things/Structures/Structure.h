#ifndef __STRUCTURE__
#define __STRUCTURE__

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

	Structure(const std::string& name, const std::string& sprite_path);
	virtual ~Structure();

	//bool operator< (const Structure& rhs) const { return mStructurePriority > rhs.mStructurePriority; }
	
	StructurePriority priority() const { return mStructurePriority; }

	virtual void update() = 0;

	int id() const { return mId; }

	int turnsToBuild() const { return mTurnsToBuild; }
	int age() const { return mAge; }
	int maxAge() const { return mMaxAge; }
	bool repairable() const { return mRepairable; }

	bool enabled() const { return mStructureState == OPERATIONAL; }
	void enabled(bool _b);

	bool idle() const { return mStructureState == IDLE; }
	void idle(bool _b);

	bool underConstruction() const { return mStructureState == UNDER_CONSTRUCTION; }

	void incrementAge() { mAge++; }

	ResourcePool& resourceValue() { return mResourceValue; }
	ResourcePool& resourcesIn() { return mResourcesInput; }
	ResourcePool& resourcesOut() { return mResourcesOutput; }

	void input(ResourcePool& _resourcePool);
	bool enoughResourcesAvailable(const ResourcePool& r);

	StructureState state() const { return mStructureState; }

	/** Indicates that the structure can act as a connector (tube) */
	bool isConnector() const { return mConnector; }

	ConnectorDir connectorDirection() const { return mConnectorDirection; }

	bool requiresCHAP() const { return mRequiresCHAP; }
	bool providesCHAP() const { return mProvidesCHAP; }
	bool selfSustained() const { return mSelfSustained; }
	bool isFactory() const { return mIsFactory; }

protected:

	void turnsToBuild(int _t) { mTurnsToBuild = _t; }
	void maxAge(int _age) { mMaxAge = _age; }
	
	/**
	 * Set the current age of the Structure.
	 * 
	 * \note	Available to reset current age to to simulate repairs to extend
	 *			the life of the Structure.
	 */
	void age(int _age) { mAge = _age; }

	void repairable(bool _r) { mRepairable = _r; }

	/// \todo Make these pure virtual
	virtual void defineResourceInput() {}
	virtual void defineResourceOutput() {}
	virtual void defineResourceValue() {}

	void activate();

	void isConnector(bool _b) { mConnector = _b; }
	void connectorDirection(ConnectorDir _cd) { mConnectorDirection = _cd; }

	void requiresCHAP(bool _b) { mRequiresCHAP = _b; }

	void providesCHAP(bool _b) { mProvidesCHAP = _b; }

	void selfSustained(bool _b) { mSelfSustained = _b; }

	void isFactory(bool _b) { mIsFactory = _b; }

	void priority(StructurePriority _sp) { mStructurePriority = _sp; }

	ResourcePool			mResourcesNeededToBuild;	/**< Resource needed to build the Structure */
	
	ResourcePool			mResourcesInput;			/**< Resources needed to operate the Structure. */
	ResourcePool			mResourcesOutput;			/**< Resources provided by the Structure if operating properly. */

	ResourcePool			mResourceValue;				/**< Resource reclamation values when dozed. */

private:
	int						mId;						/**< ID of the Structure. */

	int						mTurnsToBuild;				/**< Number of turns it takes to build the Structure. */

	int						mAge;						/**< Age of the Structure in turns. */
	int						mMaxAge;					/**< Maximum number of turns the Structure can remain in good repair. */

	StructureState			mStructureState;			/**< State the structure is in. */

	StructurePriority		mStructurePriority;			/**< Indicates the priority level that the Structure gets when processing resources. */

	ConnectorDir			mConnectorDirection;		/**< Directions available for connections. */

	bool					mRepairable;				/**< Indicates whether or not the Structure can be repaired. Useful for forcing some Structures die at the end of their life. */
	bool					mConnector;					/**< Indicates that the Structure can act as a connector (tube) */
	bool					mRequiresCHAP;				/**< Indicates that the Structure needs to have an active CHAP facilities in order to operate. */
	bool					mProvidesCHAP;				/**< Indicates that the Strcuture provides CHAP. */
	bool					mSelfSustained;				/**< Indicates that the Strcutre is self contained and can operate by itself. */
	bool					mIsFactory;					/**< Indicates that the Structure is a factory. */
};


#endif
