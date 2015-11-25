#pragma once

#include "Structure.h"

#include "../../RobotPool.h"


/**
 * FIXME:	Maybe should inherit from Resources and add the TurnsToBuild instead
 *			of using composition.
 */
struct ProductionCost
{
	ProductionCost() : TurnsToBuild(0) {}

	ProductionCost(int turns, double commonMetals, double commonMinerals, double rareMetals, double rareMinerals) : TurnsToBuild(turns)
	{
		CostPerTurn.commonMetals = commonMetals;
		CostPerTurn.commonMinerals = commonMinerals;
		CostPerTurn.rareMetals = rareMetals;
		CostPerTurn.rareMinerals = rareMinerals;
	}

	~ProductionCost() {}

	void clear()
	{
		CostPerTurn.clear();
		TurnsToBuild = 0;
	}

	Resources	CostPerTurn;
	int			TurnsToBuild;
};



/**
 * \brief	Defines the Factory interface.
 * 
 * Factory derives from Structure and provides the basic factory interface and
 * the underlying factory production code. Exactly what a factory is capable of
 * producing is up to the derived factory type.
 * 
 * \note	The Factory interface defines two public functions: resourcePool() and robotoPool().
 *			These must be called manually whenever a derived factory type is added to the game as
 *			there are no manager objects that do this (by design).
 * 
 * \warning	There are no sanity checks in the underlying production code to check if resourcePool
 *			or robotPool have been properly set. It is assumed that they have been.
 */
class Factory : public Structure
{

public:

	enum ProductionType
	{
		PRODUCTION_NONE,

		// Surface Factories
		PRODUCTION_DIGGER,
		PRODUCTION_DOZER,
		PRODUCTION_MINER
	};

	typedef vector<ProductionType> ProductionTypeList;

	Factory(const std::string& name, const std::string& sprite_path);
	virtual ~Factory();

	virtual void updateProduction();

	void resourcePool(Resources* _r) { mResourcesPool = _r; }
	void robotPool(RobotPool* _r) { mRobotPool = _r; }

	int productionTurnsToComplete() const { return mTurnsToComplete; }
	void productionTurnsToComplete(int _l) { mTurnsToComplete = _l; }

	int productionTurnsCompleted() const { return mTurnsCompleted; }
	void productionResetTurns() { mTurnsCompleted = 0; }

	ProductionType productionType() const { return mProduction; }
	void productionType(ProductionType _p);

	const ProductionTypeList& productionList() const { return mAvailableProducts; }

	const ProductionCost& productionCost(ProductionType) const;

	virtual void initFactory() = 0;

protected:

	virtual void productionComplete(ProductionType _p) {}

	void clearProduction();

	void addProduct(ProductionType _p);
	bool enoughResourcesAvailable();

	Resources* resourcePool() { return mResourcesPool; }
	RobotPool* robotPool() { return mRobotPool; }

private:

	int								mTurnsCompleted;
	int								mTurnsToComplete;

	ProductionType					mProduction;

	ProductionTypeList				mAvailableProducts;			/**< List of products that the Factory can produce. */

	Resources						mProductionInput;

	Resources*						mResourcesPool;				/**< Pointer to the player's resource pool. UGLY. */
	RobotPool*						mRobotPool;					/**< Pointer to the player's robot pool. UGLY. */
};