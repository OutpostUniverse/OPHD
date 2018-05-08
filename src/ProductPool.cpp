// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProductPool.h"

#include <NAS2D/NAS2D.h>

using namespace NAS2D;

/**
 * Space required to store a Product.
 */
std::map<ProductType, int> PRODUCT_STORAGE_VALUE =
{
	{ PRODUCT_DIGGER, 10 },
	{ PRODUCT_DOZER, 10 },
	{ PRODUCT_MINER, 10 },
	{ PRODUCT_EXPLORER, 10 },
	{ PRODUCT_TRUCK, 10 },

	{ PRODUCT_ROAD_MATERIALS, 1 },
	{ PRODUCT_MAINTENANCE_PARTS, 1 },

	{ PRODUCT_CLOTHING, 1 },
	{ PRODUCT_MEDICINE, 1 }
};


/**
 * Simple utility function that helps reduce syntax verbosity.
 */
static inline int productStorage(ProductType type)
{
	return PRODUCT_STORAGE_VALUE[type];
}


/**
 * 
 */
int ProductPool::capacity() const
{
	return mCapacity;
}


/**
 * 
 */
int ProductPool::availableStorage() const
{
	int stored = 0;
	for (size_t i = 0; i < static_cast<size_t>(PRODUCT_COUNT); ++i)
	{
		stored += mProducts[i] * productStorage(static_cast<ProductType>(i));
	}

	return mCapacity - stored;
}


/**
 * 
 */
bool ProductPool::canStore(ProductType type, int count)
{
	if (count * productStorage(type) <= availableStorage())
	{
		return true;
	}

	return false;
}


/**
 * Stores a specified amount of a ProductType.
 * 
 * \param	type	ProductType enumerator.
 * \param	count	Number of products to store.
 * 
 * \note	This is an all or nothing call. Products will only be
 *			stored if the entire specified amount will fit.
 */
void ProductPool::store(ProductType type, int count)
{
	int storageRequired = count * productStorage(type);

	if (storageRequired <= availableStorage())
	{
		mProducts[static_cast<int>(type)] += count;
		mCapacity -= storageRequired;
	}
}


/**
 * 
 */
int ProductPool::pull(ProductType type, int c)
{
	int pulledCount = clamp(c, 0, mProducts[static_cast<int>(type)]);
	mProducts[static_cast<int>(type)] -= pulledCount;

	mCapacity += pulledCount * productStorage(type);

	return pulledCount;
}


/**
 * 
 */
int ProductPool::count(ProductType type)
{
	return mProducts[static_cast<int>(type)];
}
