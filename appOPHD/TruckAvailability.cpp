#include "TruckAvailability.h"

#include "StructureManager.h"
#include "MapObjects/Structures/Warehouse.h"

#include <NAS2D/Utility.h>


int getTruckAvailability()
{
	int trucksAvailable = 0;

	const auto& warehouseList = NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto* warehouse : warehouseList)
	{
		trucksAvailable += warehouse->products().count(ProductType::PRODUCT_TRUCK);
	}

	return trucksAvailable;
}


/**
 * \return 1 on success, 0 otherwise.
 */
int pullTruckFromInventory()
{
	int trucksAvailable = getTruckAvailability();

	if (trucksAvailable == 0) { return 0; }

	const auto& warehouseList = NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto* warehouse : warehouseList)
	{
		if (warehouse->products().pull(ProductType::PRODUCT_TRUCK, 1) > 0)
		{
			return 1;
		}
	}

	return 0;
}


/**
 * \return 1 on success, 0 otherwise.
 */
int pushTruckIntoInventory()
{
	const int storageNeededForTruck = storageRequiredPerUnit(ProductType::PRODUCT_TRUCK);

	const auto& warehouseList = NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto* warehouse : warehouseList)
	{
		if (warehouse->products().availableStorage() >= storageNeededForTruck)
		{
			warehouse->products().store(ProductType::PRODUCT_TRUCK, 1);
			return 1;
		}
	}

	return 0;
}
