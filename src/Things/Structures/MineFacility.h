#pragma once

#include "Structure.h"

#include "../../Mine.h"

/**
 * Implements the Mine Facility.
 */
class MineFacility: public Structure
{
public:
	typedef NAS2D::Signals::Signal1<MineFacility*> ExtensionCompleteCallback;
public:
	MineFacility(Mine* mine);
	virtual ~MineFacility() {};
	
	void mine(Mine* _m) { mMine = _m; }
	void maxDepth(int depth) { mMaxDepth = depth; }

	bool extending() const;
	bool canExtend() const;
	void extend();

	int digTimeRemaining() const;

	/**
	 * Gets a pointer to the mine the MineFacility manages.
	 */
	Mine* mine() { return mMine; }

	ExtensionCompleteCallback& extensionComplete() { return mExtensionComplete; }

protected:
	virtual void think();

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

private:
	MineFacility() = delete;
	MineFacility(const MineFacility&) = delete;
	MineFacility& operator=(const MineFacility&) = delete;

private:
	virtual void activated() final;

private:
	int							mMaxDepth = 0;				/**< Maximum digging depth. */
	int							mDigTurnsRemaining = 0;		/**< Turns remaining before extension is complete. */

	Mine*						mMine = nullptr;			/**< Mine that this facility manages. */

	ExtensionCompleteCallback	mExtensionComplete;			/**< Called whenever an extension is completed. */
};
