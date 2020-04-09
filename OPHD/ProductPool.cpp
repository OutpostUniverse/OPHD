// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProductPool.h"

#include <algorithm>

using namespace NAS2D;
using namespace NAS2D::Xml;

/**
 * Space required to store a Product.
 */
std::map<ProductType, int> PRODUCT_STORAGE_VALUE =
{
	{ ProductType::PRODUCT_DIGGER, 10 },
	{ ProductType::PRODUCT_DOZER, 10 },
	{ ProductType::PRODUCT_MINER, 10 },
	{ ProductType::PRODUCT_EXPLORER, 10 },
	{ ProductType::PRODUCT_TRUCK, 10 },

	{ ProductType::PRODUCT_ROAD_MATERIALS, 1 },
	{ ProductType::PRODUCT_MAINTENANCE_PARTS, 1 },

	{ ProductType::PRODUCT_CLOTHING, 1 },
	{ ProductType::PRODUCT_MEDICINE, 1 }
};


/**
 * Gets the amount of storage required for one unit of a Product.
 */
inline int storageRequiredPerUnit(ProductType type)
{
	return PRODUCT_STORAGE_VALUE[type];
}


/**
 * Gets the amount of storage required for a given number of Products.
 */
int storageRequired(ProductType type, int count)
{
	return PRODUCT_STORAGE_VALUE[type] * count;
}


/**
 * Internal helper function.
 */
static int computeCurrentStorage(const ProductPool::ProductTypeCount& products)
{
	int stored = 0;

	for (size_t i = 0; i < static_cast<size_t>(ProductType::PRODUCT_COUNT); ++i)
	{
		stored +=storageRequired(static_cast<ProductType>(i), products[i]);
	}

	return stored;
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
	return mCapacity - mCurrentStorageCount;
}


/**
 * 
 */
bool ProductPool::canStore(ProductType type, int count)
{
	if (count * storageRequiredPerUnit(type) <= availableStorage())
	{
		return true;
	}

	return false;
}


bool ProductPool::empty() const
{
	return (availableStorage() == capacity());
}


bool ProductPool::atCapacity() const
{
	return (availableStorage() == 0);
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
		mProducts[static_cast<int>(type)] += count;
	}

	mCurrentStorageCount = computeCurrentStorage(mProducts);
}


/**
 * 
 */
int ProductPool::pull(ProductType type, int c)
{
	int pulledCount = std::clamp(c, 0, mProducts[static_cast<int>(type)]);
	mProducts[static_cast<int>(type)] -= pulledCount;
	mCurrentStorageCount = computeCurrentStorage(mProducts);

	return pulledCount;
}


/**
 * 
 */
int ProductPool::count(ProductType type)
{
	return mProducts[static_cast<std::size_t>(type)];
}


void ProductPool::verifyCount()
{
	mCurrentStorageCount = computeCurrentStorage(mProducts);
}


/**
 * 
 */
void ProductPool::serialize(NAS2D::Xml::XmlElement* element)
{
	element->attribute(constants::SAVE_GAME_PRODUCT_DIGGER,				count(ProductType::PRODUCT_DIGGER));
	element->attribute(constants::SAVE_GAME_PRODUCT_DOZER,				count(ProductType::PRODUCT_DOZER));
	element->attribute(constants::SAVE_GAME_PRODUCT_MINER,				count(ProductType::PRODUCT_MINER));
	element->attribute(constants::SAVE_GAME_PRODUCT_EXPLORER,			count(ProductType::PRODUCT_EXPLORER));
	element->attribute(constants::SAVE_GAME_PRODUCT_TRUCK,				count(ProductType::PRODUCT_TRUCK));
	element->attribute(constants::SAVE_GAME_PRODUCT_ROAD_MATERIALS,		count(ProductType::PRODUCT_ROAD_MATERIALS));
	element->attribute(constants::SAVE_GAME_MAINTENANCE_PARTS,			count(ProductType::PRODUCT_MAINTENANCE_PARTS));
	element->attribute(constants::SAVE_GAME_PRODUCT_CLOTHING,			count(ProductType::PRODUCT_CLOTHING));
	element->attribute(constants::SAVE_GAME_PRODUCT_MEDICINE,			count(ProductType::PRODUCT_MEDICINE));
}


/**
 * 
 */
void ProductPool::deserialize(NAS2D::Xml::XmlElement* element)
{
	/// \todo	This should probably trigger an exception.
	if (element == nullptr) { return; }

	XmlAttribute* attribute = element->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == constants::SAVE_GAME_PRODUCT_DIGGER)				{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_DIGGER)]); }
		else if (attribute->name() == constants::SAVE_GAME_PRODUCT_DOZER)			{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_DOZER)]); }
		else if (attribute->name() == constants::SAVE_GAME_PRODUCT_MINER)			{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_MINER)]); }
		else if (attribute->name() == constants::SAVE_GAME_PRODUCT_EXPLORER)		{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_EXPLORER)]); }
		else if (attribute->name() == constants::SAVE_GAME_PRODUCT_TRUCK)			{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_TRUCK)]); }
		else if (attribute->name() == constants::SAVE_GAME_PRODUCT_ROAD_MATERIALS)	{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_ROAD_MATERIALS)]); }
		else if (attribute->name() == constants::SAVE_GAME_MAINTENANCE_PARTS)		{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_MAINTENANCE_PARTS)]); }
		else if (attribute->name() == constants::SAVE_GAME_PRODUCT_CLOTHING)		{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_CLOTHING)]); }
		else if (attribute->name() == constants::SAVE_GAME_PRODUCT_MEDICINE)		{ attribute->queryIntValue(mProducts[static_cast<std::size_t>(ProductType::PRODUCT_MEDICINE)]); }

		attribute = attribute->next();
	}
	mCurrentStorageCount = computeCurrentStorage(mProducts);
}
