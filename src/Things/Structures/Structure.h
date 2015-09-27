#ifndef __STRUCTURE__
#define __STRUCTURE__

#include "../Thing.h"
#include "../../Resources.h"
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


	Structure(const std::string& name, const std::string& sprite_path);
	virtual ~Structure();

	virtual void update() = 0;

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

	Resources& resourceValue() { return mResourceValue; }
	Resources& resourcesIn() { return mResourcesInput; }
	Resources& resourcesOut() { return mResourcesOutput; }

	void input(Resources& _resourcePool);
	bool enoughResourcesAvailable(const Resources& r);

	StructureState state() const { return mStructureState; }

	bool isConnector() const { return mConnector; }

	ConnectorDir connectorDirection() const { return mConnectorDirection; }

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


	Resources	mResourcesNeededToBuild;	/**< Resource needed to build the Structure */

	Resources	mResourcesInput;			/**< Resources needed to operate the Structure. */
	Resources	mResourcesOutput;			/**< Resources provided by the Structure if operating properly. */

	Resources	mResourceValue;				/**< Resource reclamation values when dozed. */

private:
	int				mTurnsToBuild;		/**< Number of turns it takes to build the Structure. */

	int				mAge;				/**< Age of the Structure in turns. */
	int				mMaxAge;			/**< Maximum number of turns the Structure can remain in good repair. */

	StructureState	mStructureState;	/**< State the structure is in. */

	ConnectorDir	mConnectorDirection;	/**< Directions available for connections. */

	bool			mRepairable;		/**< Indicates whether or not the Structure can be repaired. Useful for forcing some Structures die at the end of their life. */
	bool			mConnector;			/**< Indicates that the Structure can act as a connector (tube) */
};


#endif
