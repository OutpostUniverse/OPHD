#pragma once

#include <libOPHD/StorableResources.h>


/**
 * Defines cost in materials per turn.
 *
 * Basically just a storage class used to contain resource costs per turn and turn count
 * needed to produce a particular item.
 */
struct ProductionCost
{
	int turnsToBuild{0};
	StorableResources resourceCost;
};
