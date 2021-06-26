#include "ProductPool.h"

#include <algorithm>


namespace {
	/**
	 * Space required to store a Product.
	 */
	const std::map<ProductType, int> ProductStorageSpace =
	{
		{ ProductType::PRODUCT_DIGGER, 10 },
		{ ProductType::PRODUCT_DOZER, 10 },
		{ ProductType::PRODUCT_MINER, 10 },
		{ ProductType::PRODUCT_EXPLORER, 10 },
		{ ProductType::PRODUCT_TRUCK, 10 },

		{ ProductType::PRODUCT_RESERVED_AG_05, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_06, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_07, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_08, 0 },

		{ ProductType::PRODUCT_MAINTENANCE_PARTS, 1 },

		{ ProductType::PRODUCT_RESERVED_AG_10, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_11, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_12, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_13, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_14, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_15, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_16, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_17, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_18, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_19, 0 },

		{ ProductType::PRODUCT_RESERVED_AG_20, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_21, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_22, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_23, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_24, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_25, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_26, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_27, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_28, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_29, 0 },

		{ ProductType::PRODUCT_RESERVED_AG_30, 0 },
		{ ProductType::PRODUCT_RESERVED_AG_31, 0 },

		{ ProductType::PRODUCT_CLOTHING, 1 },
		{ ProductType::PRODUCT_MEDICINE, 1 },

		{ ProductType::PRODUCT_RESERVED_UG_34, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_35, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_36, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_37, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_38, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_39, 0 },

		{ ProductType::PRODUCT_RESERVED_UG_40, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_41, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_42, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_43, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_44, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_45, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_46, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_47, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_48, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_49, 0 },

		{ ProductType::PRODUCT_RESERVED_UG_50, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_51, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_52, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_53, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_54, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_55, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_56, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_57, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_58, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_59, 0 },

		{ ProductType::PRODUCT_RESERVED_UG_60, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_61, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_62, 0 },
		{ ProductType::PRODUCT_RESERVED_UG_63, 0 }
	};

	/**
	 * Gets the amount of storage required for a given number of Products.
	 */
	inline int storageRequired(ProductType type, int count)
	{
		return ProductStorageSpace.at(type) * count;
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
	return ProductStorageSpace.at(type);
}


int ProductPool::capacity() const
{
	return mCapacity;
}


int ProductPool::availableStorage() const
{
	return mCapacity - mCurrentStorageCount;
}


int ProductPool::productStorageRequirement(ProductType type) const
{
	return storageRequiredPerUnit(type);
}


bool ProductPool::canStore(ProductType type, int count)
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


int ProductPool::count(ProductType type)
{
	return mProducts[static_cast<std::size_t>(type)];
}


void ProductPool::verifyCount()
{
	mCurrentStorageCount = computeCurrentStorage(mProducts);
}


void ProductPool::serialize(NAS2D::Xml::XmlElement* element)
{
	element->attribute(constants::SaveGameProductDigger, count(ProductType::PRODUCT_DIGGER));
	element->attribute(constants::SaveGameProductDozer, count(ProductType::PRODUCT_DOZER));
	element->attribute(constants::SaveGameProductMiner, count(ProductType::PRODUCT_MINER));
	element->attribute(constants::SaveGameProductExplorer, count(ProductType::PRODUCT_EXPLORER));
	element->attribute(constants::SaveGameProductTruck, count(ProductType::PRODUCT_TRUCK));
	element->attribute(constants::SaveGameProductMaintenanceParts, count(ProductType::PRODUCT_MAINTENANCE_PARTS));
	element->attribute(constants::SaveGameProductClothing, count(ProductType::PRODUCT_CLOTHING));
	element->attribute(constants::SaveGameProductMedicine, count(ProductType::PRODUCT_MEDICINE));
}


void ProductPool::deserialize(NAS2D::Xml::XmlElement* element)
{
	/// \todo	This should probably trigger an exception.
	if (element == nullptr) { return; }

	const auto* attribute = element->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == constants::SaveGameProductDigger) { attribute->queryIntValue(mProducts[ProductType::PRODUCT_DIGGER]); }
		else if (attribute->name() == constants::SaveGameProductDozer) { attribute->queryIntValue(mProducts[ProductType::PRODUCT_DOZER]); }
		else if (attribute->name() == constants::SaveGameProductMiner) { attribute->queryIntValue(mProducts[ProductType::PRODUCT_MINER]); }
		else if (attribute->name() == constants::SaveGameProductExplorer) { attribute->queryIntValue(mProducts[ProductType::PRODUCT_EXPLORER]); }
		else if (attribute->name() == constants::SaveGameProductTruck) { attribute->queryIntValue(mProducts[ProductType::PRODUCT_TRUCK]); }
		else if (attribute->name() == constants::SaveGameProductMaintenanceParts) { attribute->queryIntValue(mProducts[ProductType::PRODUCT_MAINTENANCE_PARTS]); }
		else if (attribute->name() == constants::SaveGameProductClothing) { attribute->queryIntValue(mProducts[ProductType::PRODUCT_CLOTHING]); }
		else if (attribute->name() == constants::SaveGameProductMedicine) { attribute->queryIntValue(mProducts[ProductType::PRODUCT_MEDICINE]); }

		attribute = attribute->next();
	}
	mCurrentStorageCount = computeCurrentStorage(mProducts);
}
