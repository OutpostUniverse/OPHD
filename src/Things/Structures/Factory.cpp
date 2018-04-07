// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <algorithm>

#include "Factory.h"

typedef std::map<ProductType, ProductionCost> ProductionTypeTable;


/**
 * Table with production information for each product that factories can produce.
 *
 * \note	This table defines parameters for -all- products that any factory can
 *			produce. It is up to the individual factory to determine what they are
 *			allowed to build.
 */
ProductionTypeTable PRODUCTION_TYPE_TABLE = 
{
	{ PRODUCT_DIGGER, ProductionCost(5, 10, 5, 5, 2) },
	{ PRODUCT_DOZER, ProductionCost(5, 10, 5, 5, 2) },
	{ PRODUCT_EXPLORER, ProductionCost(5, 10, 5, 5, 2) },
	{ PRODUCT_MINER, ProductionCost(5, 10, 5, 5, 2) },
	{ PRODUCT_ROAD_MATERIALS, ProductionCost(2, 2, 5, 0, 0) },
	{ PRODUCT_TRUCK, ProductionCost(5, 10, 5, 5, 2) },

	{ PRODUCT_MAINTENANCE_PARTS, ProductionCost(2, 2, 2, 1, 1) },

	{ PRODUCT_CLOTHING, ProductionCost(1, 0, 1, 0, 0) },
	{ PRODUCT_MEDICINE, ProductionCost(1, 0, 2, 0, 1) },
};


Factory::Factory(const std::string& name, const std::string& sprite_path):	Structure(name, sprite_path, CLASS_FACTORY)
{}


Factory::~Factory()
{}


void Factory::productType(ProductType _p)
{
	if (_p == mProduct) { return; }

	if (_p == PRODUCT_NONE)
	{
		clearProduction();
		return;
	}

	// Ensure we're only setting a product that the Factory is capable of producing.
	auto it = find(mAvailableProducts.begin(), mAvailableProducts.end(), _p);
	if (it == mAvailableProducts.end())
	{
		return;
	}

	mProduct = _p;

	productionResetTurns();

	mTurnsToComplete = PRODUCTION_TYPE_TABLE[mProduct].turnsToBuild();
}


const ProductionCost& Factory::productCost(ProductType _pt) const
{
	// Sanity check
	if (PRODUCTION_TYPE_TABLE.empty()) { throw std::runtime_error("Factory::productionCost() called before production table filled!"); }

	return PRODUCTION_TYPE_TABLE[_pt];
}


void Factory::updateProduction()
{
	if (state() != StructureState::OPERATIONAL)
	{
		return;
	}

	if (mProduct == PRODUCT_NONE || !enoughResourcesAvailable())
	{
		return;
	}

	mResourcesPool->commonMetals(mResourcesPool->commonMetals() - PRODUCTION_TYPE_TABLE[mProduct].commonMetals());
	mResourcesPool->commonMinerals(mResourcesPool->commonMinerals() - PRODUCTION_TYPE_TABLE[mProduct].commonMinerals());
	mResourcesPool->rareMetals(mResourcesPool->rareMetals() - PRODUCTION_TYPE_TABLE[mProduct].rareMetals());
	mResourcesPool->rareMinerals(mResourcesPool->rareMinerals() - PRODUCTION_TYPE_TABLE[mProduct].rareMinerals());

	++mTurnsCompleted;

	if (mTurnsCompleted > mTurnsToComplete)
	{
		productionResetTurns();
		productionComplete(mProduct);
	}

}


void Factory::productionComplete(ProductType _p)
{
	mProductionComplete(_p, *this);
}


bool Factory::enoughResourcesAvailable()
{
	#ifdef _DEBUG
	if (mResourcesPool == nullptr) { throw std::runtime_error("Factory::enoughResourcesAvailable() called with a null Resource Pool set"); }
	#endif

	if (mResourcesPool->commonMetals() >= PRODUCTION_TYPE_TABLE[mProduct].commonMetals() &&
		mResourcesPool->commonMinerals() >= PRODUCTION_TYPE_TABLE[mProduct].commonMinerals() &&
		mResourcesPool->rareMetals() >= PRODUCTION_TYPE_TABLE[mProduct].rareMetals() &&
		mResourcesPool->rareMinerals() >= PRODUCTION_TYPE_TABLE[mProduct].rareMinerals())
	{
		return true;
	}

	return false;
}


void Factory::addProduct(ProductType _p)
{
	if (find(mAvailableProducts.begin(), mAvailableProducts.end(), _p) != mAvailableProducts.end())
	{
		return;
	}

	mAvailableProducts.push_back(_p);
}


void Factory::clearProduction()
{
	mTurnsCompleted = 0;
	mTurnsToComplete = 0;
	mProduct = PRODUCT_NONE;
}
