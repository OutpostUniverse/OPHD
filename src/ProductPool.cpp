#include "ProductPool.h"

#include <NAS2D/NAS2D.h>

using namespace NAS2D;

/**
 * Space required to store a Product.
 * 
 * \note	For simplicity's sake this is arranged in the same order
 *			as the ProductType enumeration. Ensure that if the order
 *			of that enumeration that this order is updated accordingly.
 */
std::array<int, PRODUCT_COUNT>	PRODUCT_STORAGE_VALUE = 
{
	0,		// PRODUCT_NONE
	10,		// PRODUCT_DIGGER
	10,		// PRODUCT_DOZER
	10		// PRODUCT_MINER
};


/**
 * 
 */
int productStorage(ProductType type)
{
	return PRODUCT_STORAGE_VALUE[static_cast<int>(type)];
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
int ProductPool::_availableStorage() const
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
	if (count * productStorage(type) <= _availableStorage())
	{
		return true;
	}

	return false;
}


/**
 * 
 */
void ProductPool::store(ProductType type, int count)
{
	int storageRequired = count * productStorage(type);

	if (storageRequired <= _availableStorage())
	{
		mProducts[static_cast<int>(type)] += count;
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
