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


const ProductionCost& productCost(ProductType _pt)
{
	// Sanity check
	if (PRODUCTION_TYPE_TABLE.empty()) { throw std::runtime_error("Factory::productionCost() called before production table filled!"); }

	return PRODUCTION_TYPE_TABLE[_pt];
}



Factory::Factory(const std::string& name, const std::string& spritePath) :	Structure(name, spritePath, CLASS_FACTORY)
{}


void Factory::productType(ProductType type)
{
	if (type == mProduct) { return; }

	if (type == PRODUCT_NONE)
	{
		clearProduction();
		return;
	}

	// Ensure we're only setting a product that the Factory is capable of producing.
	auto it = find(mAvailableProducts.begin(), mAvailableProducts.end(), type);
	if (it == mAvailableProducts.end())
	{
		return;
	}

	mProduct = type;

	productionResetTurns();

	mTurnsToComplete = PRODUCTION_TYPE_TABLE[mProduct].turnsToBuild();
}


/**
 * Pulls a product that is in the waiting queue.
 * 
 * \throws	std::runtime_error if the product waiting queue is empty.
 */
ProductType Factory::pullProduct()
{
	if (mProductWaiting == PRODUCT_NONE)
	{
		/**
		 * Exception here is overkill but this indicates a logic error
		 * somewhere else in the code and should fail as loudly as possible.
		 */
		throw std::runtime_error("Factory::pullProduct(): Pull product called with an empty product queue!");
	}

	ProductType returnProduct = mProductWaiting;
	mProductWaiting = PRODUCT_NONE;
	return returnProduct;
}


/**
 * Production update loop.
 */
void Factory::updateProduction()
{
	/**
	 * A few check conditions to see if we're doing any processing.
	 * 
	 * \fixme	Most of these can be combined into a single
	 *			compound conditional statement.
	 */
	if (state() != StructureState::OPERATIONAL)
	{
		return;
	}

	if (mProduct == PRODUCT_NONE)
	{
		return;
	}

	if (mProductWaiting != PRODUCT_NONE)
	{
		mProductionComplete(*this);
		idle(IdleReason::IDLE_FACTORY_PRODUCTION_COMPLETE);
		return;
	}

	if (!enoughResourcesAvailable())
	{
		idle(IdleReason::IDLE_FACTORY_INSUFFICIENT_RESOURCES);
		return;
	}
	
	/**
	 * Using explicit calls here instead of ResourcePool::operator-=() because the production
	 * table doesn't use ResourcePool objects.
	 * 
	 * \todo	Add an operator-=() to ResourcePool that takes type ProductionCost to improve
	 *			readability of below code. Not thrilled with polluting the namespace but eh.
	 */
	mResourcesPool->commonMetals(mResourcesPool->commonMetals() - PRODUCTION_TYPE_TABLE[mProduct].commonMetals());
	mResourcesPool->commonMinerals(mResourcesPool->commonMinerals() - PRODUCTION_TYPE_TABLE[mProduct].commonMinerals());
	mResourcesPool->rareMetals(mResourcesPool->rareMetals() - PRODUCTION_TYPE_TABLE[mProduct].rareMetals());
	mResourcesPool->rareMinerals(mResourcesPool->rareMinerals() - PRODUCTION_TYPE_TABLE[mProduct].rareMinerals());

	++mTurnsCompleted;

	if (mTurnsCompleted > mTurnsToComplete)
	{
		productionResetTurns();
		mProductWaiting = mProduct;
		mProductionComplete(*this);
	}
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


void Factory::addProduct(ProductType type)
{
	if (find(mAvailableProducts.begin(), mAvailableProducts.end(), type) != mAvailableProducts.end())
	{
		return;
	}

	mAvailableProducts.push_back(type);
}


void Factory::clearProduction()
{
	mTurnsCompleted = 0;
	mTurnsToComplete = 0;
	mProduct = PRODUCT_NONE;
}
