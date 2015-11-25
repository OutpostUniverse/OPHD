#include "Factory.h"



struct ProductionValues
{
	ProductionValues() : TurnsToBuild(0) {}

	ProductionValues(int turns, double commonMetals, double commonMinerals, double rareMetals, double rareMinerals) : TurnsToBuild(turns)
	{
		CostPerTurn.commonMetals = commonMetals;
		CostPerTurn.commonMinerals = commonMinerals;
		CostPerTurn.rareMetals = rareMetals;
		CostPerTurn.rareMinerals = rareMinerals;
	}

	~ProductionValues() {}

	Resources	CostPerTurn;
	int			TurnsToBuild;
};


typedef map<Factory::ProductionType, ProductionValues> ProductionTypeTable;

ProductionTypeTable		PRODUCTION_TYPE_TABLE;
bool					FACTORY_PRODUCTION_TABLE_FILLED = false;


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
	if (FACTORY_PRODUCTION_TABLE_FILLED)
		return;
	else
		FACTORY_PRODUCTION_TABLE_FILLED = true;

	PRODUCTION_TYPE_TABLE[Factory::PRODUCTION_DIGGER] = ProductionValues(6, 10.0f, 5.0f, 5.0f, 2.0f);
	PRODUCTION_TYPE_TABLE[Factory::PRODUCTION_DOZER] = ProductionValues(6, 10.0f, 5.0f, 5.0f, 2.0f);
	PRODUCTION_TYPE_TABLE[Factory::PRODUCTION_MINER] = ProductionValues(6, 10.0f, 5.0f, 5.0f, 2.0f);
}


Factory::Factory(const string& name, const string& sprite_path):	Structure(name, sprite_path),
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
		mProduction = _p;
		return;
	}

	// Ensure we're only setting a product that the Factory is capable of producing.
	auto it = find(mAvailableProducts.begin(), mAvailableProducts.end(), _p);
	if (it == mAvailableProducts.end())
		return;

	mProduction = _p;

	resetTurns();

	mTurnsToComplete = PRODUCTION_TYPE_TABLE[mProduction].TurnsToBuild;
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
		resetTurns();
		productionComplete(mProduction);
	}

}


bool Factory::enoughResourcesAvailable()
{
	if (mResourcesPool->commonMetals >= mProductionInput.commonMetals &&
		mResourcesPool->rareMetals >= mProductionInput.rareMetals &&
		mResourcesPool->commonMinerals >= mProductionInput.commonMinerals &&
		mResourcesPool->rareMinerals >= mProductionInput.rareMinerals)
		return true;

	return false;
}


void Factory::addProduct(ProductionType _p)
{
	if (find(mAvailableProducts.begin(), mAvailableProducts.end(), _p) != mAvailableProducts.end())
		return;

	mAvailableProducts.push_back(_p);
}