#pragma once

#include "../Structure.h"

#include <NAS2D/Signal/Signal.h>


struct StorableResources;
class OreDeposit;


class MineFacility : public Structure
{
public:
	using ExtensionCompleteSignal = NAS2D::Signal<MineFacility*>;
public:
	MineFacility(OreDeposit* oreDeposit);

	void oreDeposit(OreDeposit* oreDeposit);
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

	ExtensionCompleteSignal::Source& extensionComplete();

protected:
	friend class MapViewState;

	StorableResources maxTransferAmounts();

	void assignedTrucks(int count);
	void digTimeRemaining(int count);

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

	OreDeposit* mOreDeposit = nullptr; /**< OreDeposit that this facility manages. */

	ExtensionCompleteSignal mExtensionComplete; /**< Called whenever an extension is completed. */
};
