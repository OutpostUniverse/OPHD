#include "Factory.h"

#include "../../States/MapViewStateHelper.h" // yuck

#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/ProductionCost.h>

#include <NAS2D/Dictionary.h>

#include <map>
#include <algorithm>
#include <stdexcept>


namespace
{
	void assertNoDuplicates(const std::vector<ProductType>& products)
	{
		for (std::size_t i = 0; i < products.size(); ++i)
		{
			for (std::size_t j = i + 1; j < products.size(); ++j)
			{
				if (products[i] == products[j])
				{
					throw std::runtime_error("Duplicate product added to factory list: " + std::to_string(products[j]));
				}
			}
		}
	}


	/**
	 * Table with production information for each product that factories can produce.
	 *
	 * \note	This table defines parameters for -all- products that any factory can
	 *			produce. It is up to the individual factory to determine what they are
	 *			allowed to build.
	 */
	const std::map<ProductType, ProductionCost> ProductionCostTable =
	{
		{ProductType::PRODUCT_NONE, ProductionCost{}},

		{ProductType::PRODUCT_DIGGER, ProductionCost{5, {3, 1, 1, 0}}},
		{ProductType::PRODUCT_DOZER, ProductionCost{5, {3, 1, 1, 0}}},
		{ProductType::PRODUCT_EXPLORER, ProductionCost{5, {5, 2, 1, 1}}},
		{ProductType::PRODUCT_MINER, ProductionCost{5, {3, 2, 1, 1}}},
		{ProductType::PRODUCT_TRUCK, ProductionCost{3, {2, 1, 1, 0}}},

		{ProductType::PRODUCT_MAINTENANCE_PARTS, ProductionCost{2, {2, 2, 1, 1}}},

		{ProductType::PRODUCT_CLOTHING, ProductionCost{1, {0, 1, 0, 0}}},
		{ProductType::PRODUCT_MEDICINE, ProductionCost{1, {0, 2, 0, 1}}},
	};
}


const ProductionCost& productCost(ProductType productType)
{
	return ProductionCostTable.at(productType);
}


Factory::Factory(StructureID id, std::vector<ProductType> products, Tile& tile) :
	Structure{id, tile},
	mProduct{ProductType::PRODUCT_NONE},
	mProductWaiting{ProductType::PRODUCT_NONE},
	mAvailableProducts{std::move(products)}
{
	assertNoDuplicates(mAvailableProducts);
}


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

	mTurnsToComplete = productCost(mProduct).turnsToBuild;
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
		if (mProductionCompleteHandler) { mProductionCompleteHandler(*this); }
		idle(IdleReason::FactoryProductionComplete);
		return;
	}

	if (!enoughResourcesAvailable())
	{
		idle(IdleReason::FactoryInsufficientResources);
		return;
	}

	const auto& productionCost = productCost(mProduct);
	auto cost = productionCost.resourceCost;

	removeRefinedResources(cost);

	if (!cost.isEmpty()) { throw std::runtime_error("Factory::updateProduction(): Production cost not empty"); }

	++mTurnsCompleted;

	if (mTurnsCompleted > mTurnsToComplete)
	{
		productionResetTurns();
		mProductWaiting = mProduct;
		if (mProductionCompleteHandler) { mProductionCompleteHandler(*this); }
	}
}


void Factory::productionCompleteHandler(ProductionCompleteDelegate newProductionCompleteHandler)
{
	mProductionCompleteHandler = newProductionCompleteHandler;
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

	const auto& productionCost = productCost(mProduct);
	return (*mResources >= productionCost.resourceCost);
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
