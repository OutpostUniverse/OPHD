#include "ProductPool.h"

#include "Constants/Numbers.h"
#include "Constants/Strings.h"

#include <libOPHD/ProductCatalog.h>

#include <NAS2D/Dictionary.h>
#include <NAS2D/ParserHelper.h>

#include <algorithm>


namespace
{
	constexpr int BaseProductCapacity{100};

	/**
	 * Gets the amount of storage required for a given number of Products.
	 */
	inline int storageRequired(ProductType type, int count)
	{
		return ProductCatalog::has(type) ? ProductCatalog::get(type).storageSpace * count : 0;
	}


	int computeCurrentStorage(const ProductPool::ProductTypeCount& products)
	{
		int stored = 0;
		for (std::size_t i = 0; i < static_cast<std::size_t>(ProductType::PRODUCT_COUNT); ++i)
		{
			stored += storageRequired(static_cast<ProductType>(i), products[i]);
		}
		return stored;
	}
}

/**
 * Gets the amount of storage required for one unit of a Product.
 */
int storageRequiredPerUnit(ProductType type)
{
	return ProductCatalog::get(type).storageSpace;
}


ProductPool::ProductPool() :
	mCapacity{BaseProductCapacity}
{
}


int ProductPool::capacity() const
{
	return mCapacity;
}


int ProductPool::availableStorage() const
{
	return mCapacity - mCurrentStorageCount;
}


int ProductPool::availableStoragePercent() const
{
	return (100 - (mCurrentStorageCount * 100 / mCapacity));
}


int ProductPool::productStorageRequirement(ProductType type) const
{
	return storageRequiredPerUnit(type);
}


bool ProductPool::canStore(ProductType type, int count) const
{
	return count * storageRequiredPerUnit(type) <= availableStorage();
}


bool ProductPool::empty() const
{
	return (availableStorage() == capacity());
}


bool ProductPool::atCapacity() const
{
	return (availableStorage() == 0);
}


// Attempts to transfer all or partial storage to another pool
void ProductPool::transferAllTo(ProductPool& destination)
{
	if (empty() || destination.atCapacity()) { return; }

	for (std::size_t i = 0; i < ProductType::PRODUCT_COUNT; ++i)
	{
		if (destination.availableStorage() == 0) { return; }

		const auto productType = static_cast<ProductType>(i);
		const bool canTransferAll = (destination.availableStorage() >= storageRequired(productType, mProducts[i]));
		const int unitsToMove = canTransferAll ? mProducts[i] : destination.availableStorage() / storageRequiredPerUnit(productType);
		destination.store(productType, unitsToMove);
		pull(productType, unitsToMove);
	}
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
	if (storageRequired(type, count) <= availableStorage())
	{
		mProducts[static_cast<std::size_t>(type)] += count;
	}

	mCurrentStorageCount = computeCurrentStorage(mProducts);
}


int ProductPool::pull(ProductType type, int c)
{
	int pulledCount = std::clamp(c, 0, mProducts[static_cast<std::size_t>(type)]);
	mProducts[static_cast<std::size_t>(type)] -= pulledCount;
	mCurrentStorageCount = computeCurrentStorage(mProducts);

	return pulledCount;
}


int ProductPool::count(ProductType type) const
{
	return mProducts[static_cast<std::size_t>(type)];
}


void ProductPool::verifyCount()
{
	mCurrentStorageCount = computeCurrentStorage(mProducts);
}


NAS2D::Dictionary ProductPool::serialize() const
{
	return NAS2D::Dictionary{{
		{constants::SaveGameProductDigger, count(ProductType::PRODUCT_DIGGER)},
		{constants::SaveGameProductDozer, count(ProductType::PRODUCT_DOZER)},
		{constants::SaveGameProductMiner, count(ProductType::PRODUCT_MINER)},
		{constants::SaveGameProductExplorer, count(ProductType::PRODUCT_EXPLORER)},
		{constants::SaveGameProductTruck, count(ProductType::PRODUCT_TRUCK)},
		{constants::SaveGameProductMaintenanceParts, count(ProductType::PRODUCT_MAINTENANCE_PARTS)},
		{constants::SaveGameProductClothing, count(ProductType::PRODUCT_CLOTHING)},
		{constants::SaveGameProductMedicine, count(ProductType::PRODUCT_MEDICINE)},
	}};
}


void ProductPool::deserialize(const NAS2D::Dictionary& dictionary)
{
	const auto required = std::vector<std::string>{constants::SaveGameProductDigger, constants::SaveGameProductDozer, constants::SaveGameProductMiner, constants::SaveGameProductExplorer, constants::SaveGameProductTruck, constants::SaveGameProductMaintenanceParts, constants::SaveGameProductClothing, constants::SaveGameProductMedicine};
	NAS2D::reportMissingOrUnexpected(dictionary.keys(), required, {});

	mProducts[ProductType::PRODUCT_DIGGER] = dictionary.get<int>(constants::SaveGameProductDigger);
	mProducts[ProductType::PRODUCT_DOZER] = dictionary.get<int>(constants::SaveGameProductDozer);
	mProducts[ProductType::PRODUCT_MINER] = dictionary.get<int>(constants::SaveGameProductMiner);
	mProducts[ProductType::PRODUCT_EXPLORER] = dictionary.get<int>(constants::SaveGameProductExplorer);
	mProducts[ProductType::PRODUCT_TRUCK] = dictionary.get<int>(constants::SaveGameProductTruck);
	mProducts[ProductType::PRODUCT_MAINTENANCE_PARTS] = dictionary.get<int>(constants::SaveGameProductMaintenanceParts);
	mProducts[ProductType::PRODUCT_CLOTHING] = dictionary.get<int>(constants::SaveGameProductClothing);
	mProducts[ProductType::PRODUCT_MEDICINE] = dictionary.get<int>(constants::SaveGameProductMedicine);

	mCurrentStorageCount = computeCurrentStorage(mProducts);
}
