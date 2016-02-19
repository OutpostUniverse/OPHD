#include "Factory.h"

typedef map<Factory::ProductionType, ProductionCost> ProductionTypeTable;

ProductionTypeTable		PRODUCTION_TYPE_TABLE;


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

	PRODUCTION_TYPE_TABLE[Factory::PRODUCTION_DIGGER] = ProductionCost(6, 10, 5, 5, 2);
	PRODUCTION_TYPE_TABLE[Factory::PRODUCTION_DOZER] = ProductionCost(6, 10, 5, 5, 2);
	PRODUCTION_TYPE_TABLE[Factory::PRODUCTION_MINER] = ProductionCost(6, 10, 5, 5, 2);
}


Factory::Factory(const string& name, const string& sprite_path):	Structure(name, sprite_path, STRUCTURE_FACTORY),
																	mTurnsCompleted(0),
																	mTurnsToComplete(0),
																	mProduction(PRODUCTION_NONE),
																	mResourcesPool(nullptr),
																	mRobotPool(nullptr)
{
	fillTable();
}


Factory::~Factory()
{}


void Factory::productionType(ProductionType _p)
{
	if (_p == mProduction)
		return;

	if (_p == Factory::PRODUCTION_NONE)
	{
		clearProduction();
		return;
	}

	// Ensure we're only setting a product that the Factory is capable of producing.
	auto it = find(mAvailableProducts.begin(), mAvailableProducts.end(), _p);
	if (it == mAvailableProducts.end())
		return;

	mProduction = _p;

	productionResetTurns();

	mTurnsToComplete = PRODUCTION_TYPE_TABLE[mProduction].TurnsToBuild;
}


const ProductionCost& Factory::productionCost(ProductionType _pt) const
{
	// Sanity check
	if (PRODUCTION_TYPE_TABLE.empty())
		throw Exception(0, "Empty Production Table", "Factory::productionCost() called before production table filled!");

	return PRODUCTION_TYPE_TABLE[_pt];
}


void Factory::updateProduction()
{
	if (state() != StructureState::OPERATIONAL)
		return;


	if (mProduction == PRODUCTION_NONE || !enoughResourcesAvailable())
	{
		return;
	}

	// Ugly dereference... better way to do it?
	*mResourcesPool -= PRODUCTION_TYPE_TABLE[mProduction].CostPerTurn;
	++mTurnsCompleted;

	if (mTurnsCompleted >= mTurnsToComplete)
	{
		productionResetTurns();
		productionComplete(mProduction);
	}

}


bool Factory::enoughResourcesAvailable()
{
	return *mResourcesPool >= PRODUCTION_TYPE_TABLE[mProduction].CostPerTurn;
}


void Factory::addProduct(ProductionType _p)
{
	if (find(mAvailableProducts.begin(), mAvailableProducts.end(), _p) != mAvailableProducts.end())
		return;

	mAvailableProducts.push_back(_p);
}


void Factory::clearProduction()
{
	mTurnsCompleted = 0;
	mTurnsToComplete = 0;
	mProduction = PRODUCTION_NONE;
}
