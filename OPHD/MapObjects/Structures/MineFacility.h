#pragma once

#include "../Structure.h"

#include "../../Mine.h"

struct StorableResources;


/**
 * Implements the Mine Facility.
 */
class MineFacility : public Structure
{
public:
	using ExtensionCompleteSignal = NAS2D::Signal<MineFacility*>;
public:
	MineFacility(Mine* mine);

	void mine(Mine* mine) { mMine = mine; }
	void maxDepth(int depth) { mMaxDepth = depth; }

	bool extending() const;
	bool canExtend() const;
	void extend();

	int digTimeRemaining() const;

	int assignedTrucks() const { return mAssignedTrucks; }
	int maxTruckCount() const { return mMaxTruckCount; }

	void addTruck() { mAssignedTrucks = std::clamp(mAssignedTrucks + 1, 1, mMaxTruckCount); }
	void removeTruck() { mAssignedTrucks = std::clamp(mAssignedTrucks - 1, 1, mMaxTruckCount); }

	/**
	 * Gets a pointer to the mine the MineFacility manages.
	 */
	Mine* mine() { return mMine; }

	ExtensionCompleteSignal::Source& extensionComplete() { return mExtensionComplete; }

protected:
	friend class MapViewState;

	StorableResources maxTransferAmounts();

	void assignedTrucks(int count) { mAssignedTrucks = count; }
	void digTimeRemaining(int count) { mDigTurnsRemaining = count; }

protected:
	void think() override;

private:
	MineFacility() = delete;
	MineFacility(const MineFacility&) = delete;
	MineFacility& operator=(const MineFacility&) = delete;

private:
	void activated() override;

private:
	int mMaxDepth = 0; /**< Maximum digging depth. */
	int mDigTurnsRemaining = 0; /**< Turns remaining before extension is complete. */
	int mAssignedTrucks = 1; /**< All mine facilities are built with at least one truck. */
	int mMaxTruckCount = 10;

	Mine* mMine = nullptr; /**< Mine that this facility manages. */

	ExtensionCompleteSignal mExtensionComplete; /**< Called whenever an extension is completed. */
};
