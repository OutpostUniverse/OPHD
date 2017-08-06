#include <algorithm>

#include "Factory.h"

typedef std::map<Factory::ProductType, ProductionCost> ProductionTypeTable;

ProductionTypeTable PRODUCTION_TYPE_TABLE;


/**
 * Utility function that fills out a table with production information for each product
 * that factories can produce.
 *
 * \note	This function defines parameters for -all- products that any factory can
 *			produce. It is up to the individual factory to determine what they are
 *			allowed to build.
 */
void fillTable()
{
	if (!PRODUCTION_TYPE_TABLE.empty())
		return;

	PRODUCTION_TYPE_TABLE[Factory::PRODUCT_DIGGER] = ProductionCost(5, 10, 5, 5, 2);
	PRODUCTION_TYPE_TABLE[Factory::PRODUCT_DOZER] = ProductionCost(5, 10, 5, 5, 2);
	PRODUCTION_TYPE_TABLE[Factory::PRODUCT_MINER] = ProductionCost(5, 10, 5, 5, 2);
}


Factory::Factory(const std::string& name, const std::string& sprite_path):	Structure(name, sprite_path, CLASS_FACTORY),
																	mTurnsCompleted(0),
																	mTurnsToComplete(0),
																	mProduct(PRODUCT_NONE),
																	mResourcesPool(nullptr)
{
	fillTable();
}


Factory::~Factory()
{}


void Factory::productType(ProductType _p)
{
	if (_p == mProduct)
		return;

	if (_p == Factory::PRODUCT_NONE)
	{
		clearProduction();
		return;
	}

	// Ensure we're only setting a product that the Factory is capable of producing.
	auto it = find(mAvailableProducts.begin(), mAvailableProducts.end(), _p);
	if (it == mAvailableProducts.end())
		return;

	mProduct = _p;

	productionResetTurns();

	mTurnsToComplete = PRODUCTION_TYPE_TABLE[mProduct].TurnsToBuild;
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

	*mResourcesPool -= PRODUCTION_TYPE_TABLE[mProduct].CostPerTurn;
	++mTurnsCompleted;

	if (mTurnsCompleted > mTurnsToComplete)
	{
		productionResetTurns();
		productionComplete(mProduct);
	}

}


void Factory::productionComplete(ProductType _p)
{
	mProductionComplete(_p, id());
}


bool Factory::enoughResourcesAvailable()
{
	#ifdef _DEBUG
	if (mResourcesPool == nullptr) { throw std::runtime_error("Factory::enoughResourcesAvailable() called with a null Resource Pool set"); }
	#endif

	return *mResourcesPool >= PRODUCTION_TYPE_TABLE[mProduct].CostPerTurn;
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
