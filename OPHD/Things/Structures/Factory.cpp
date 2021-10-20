#include "Factory.h"

#include "../../ProductionCost.h"
#include "../../States/MapViewStateHelper.h" // yuck

#include <algorithm>


using ProductionTypeTable = std::map<ProductType, ProductionCost>;


/**
 * Table with production information for each product that factories can produce.
 *
 * \note	This table defines parameters for -all- products that any factory can
 *			produce. It is up to the individual factory to determine what they are
 *			allowed to build.
 */
const ProductionTypeTable PRODUCTION_TYPE_TABLE =
{
	{ProductType::PRODUCT_NONE, ProductionCost{}},

	{ProductType::PRODUCT_DIGGER, ProductionCost{5, {10, 5, 5, 2}}},
	{ProductType::PRODUCT_DOZER, ProductionCost{5, {10, 5, 5, 2}}},
	{ProductType::PRODUCT_EXPLORER, ProductionCost{5, {10, 5, 5, 2}}},
	{ProductType::PRODUCT_MINER, ProductionCost{5, {10, 5, 5, 2}}},
	{ProductType::PRODUCT_TRUCK, ProductionCost{3, {6, 3, 2, 1}}},

	{ProductType::PRODUCT_MAINTENANCE_PARTS, ProductionCost{2, {2, 2, 1, 1}}},

	{ProductType::PRODUCT_CLOTHING, ProductionCost{1, {0, 1, 0, 0}}},
	{ProductType::PRODUCT_MEDICINE, ProductionCost{1, {0, 2, 0, 1}}},
};


const ProductionCost& productCost(ProductType productType)
{
	return PRODUCTION_TYPE_TABLE.at(productType);
}


Factory::Factory(const std::string& name, const std::string& spritePath, StructureID id) :
	Structure(name, spritePath, StructureClass::Factory, id)
{}


void Factory::productType(ProductType type)
{
	if (type == mProduct) { return; }

	if (type == ProductType::PRODUCT_NONE)
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

	mTurnsToComplete = PRODUCTION_TYPE_TABLE.at(mProduct).turnsToBuild;
}


/**
 * Pulls a product that is in the waiting queue.
 * 
 * \throws	std::runtime_error if the product waiting queue is empty.
 */
ProductType Factory::pullProduct()
{
	if (mProductWaiting == ProductType::PRODUCT_NONE)
	{
		/**
		 * Exception here is overkill but this indicates a logic error
		 * somewhere else in the code and should fail as loudly as possible.
		 */
		throw std::runtime_error("Factory::pullProduct(): Pull product called with an empty product queue!");
	}

	ProductType returnProduct = mProductWaiting;
	mProductWaiting = ProductType::PRODUCT_NONE;
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
	if (state() != StructureState::Operational)
	{
		return;
	}

	if (mProduct == ProductType::PRODUCT_NONE)
	{
		return;
	}

	if (mProductWaiting != ProductType::PRODUCT_NONE)
	{
		mProductionComplete(*this);
		idle(IdleReason::FactoryProductionComplete);
		return;
	}

	if (!enoughResourcesAvailable())
	{
		idle(IdleReason::FactoryInsufficientResources);
		return;
	}

	const auto& productionCost = PRODUCTION_TYPE_TABLE.at(mProduct);
	auto cost = productionCost.resourceCost;

	removeRefinedResources(cost);

	if (!cost.isEmpty()) { throw std::runtime_error("Factory::updateProduction(): Production cost not empty"); }

	++mTurnsCompleted;

	if (mTurnsCompleted > mTurnsToComplete)
	{
		productionResetTurns();
		mProductWaiting = mProduct;
		mProductionComplete(*this);
	}
}


NAS2D::Dictionary Factory::getDataDict() const
{
	auto dictionary = Structure::getDataDict();
	dictionary.set("production_completed", mTurnsCompleted);
	dictionary.set("production_type", mProduct);
	return dictionary;
}


bool Factory::enoughResourcesAvailable()
{
	if (mResources == nullptr) { throw std::runtime_error("Factory::enoughResourcesAvailable() called with a null Resource Pool set"); }

	const auto& productionCost = PRODUCTION_TYPE_TABLE.at(mProduct);
	if (*mResources >= productionCost.resourceCost)
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
	mProduct = ProductType::PRODUCT_NONE;
}
