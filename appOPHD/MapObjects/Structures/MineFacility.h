#pragma once

#include "../Structure.h"

#include <NAS2D/Signal/Delegate.h>


struct StorableResources;
class OreDeposit;
class Tile;


class MineFacility : public Structure
{
public:
	using ExtensionCompleteDelegate = NAS2D::Delegate<void(MineFacility*)>;

public:
	MineFacility(Tile& tile);

	void maxDepth(int depth);

	bool extending() const;
	bool canExtend() const;
	void extend();

	int digTimeRemaining() const;

	int assignedTrucks() const;
	int maxTruckCount() const;

	void addTruck();
	void removeTruck();
	OreDeposit& oreDeposit();
	const OreDeposit& oreDeposit() const;

	void extensionCompleteHandler(ExtensionCompleteDelegate newExtensionCompleteHandler);

protected:
	friend class MapViewState;

	StorableResources maxCapacity() const;
	StorableResources maxTransferAmounts() const;

	void assignedTrucks(int count);
	void digTimeRemaining(int count);

protected:
	void think() override;

private:
	MineFacility() = delete;
	MineFacility(const MineFacility&) = delete;
	MineFacility& operator=(const MineFacility&) = delete;

private:
	int mMaxDepth = 0; /**< Maximum digging depth. */
	int mDigTurnsRemaining = 0; /**< Turns remaining before extension is complete. */
	int mAssignedTrucks = 1; /**< All mine facilities are built with at least one truck. */
	int mMaxTruckCount = 10;

	OreDeposit* mOreDeposit = nullptr; /**< OreDeposit that this facility manages. */

	ExtensionCompleteDelegate mExtensionCompleteHandler; /**< Called whenever an extension is completed. */
};
