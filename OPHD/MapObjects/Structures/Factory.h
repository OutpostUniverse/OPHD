#pragma once

#include "../Structure.h"

#include "../../StorableResources.h"


struct ProductionCost;


/**
 * Defines the Factory interface.
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
	// Signal providing what was complete and a reference to the Factory.
	using ProductionSignal = NAS2D::Signal<Factory&>;

	using ProductionTypeList = std::vector<ProductType>;

public:
	Factory(StructureID id);

	virtual void updateProduction();

	void resourcePool(const StorableResources* resources) { mResources = resources; }

	int productionTurnsToComplete() const { return mTurnsToComplete; }
	void productionTurnsToComplete(int newTurnsToComplete) { mTurnsToComplete = newTurnsToComplete; }

	int productionTurnsCompleted() const { return mTurnsCompleted; }
	void productionTurnsCompleted(int newTurnsCompleted) { mTurnsCompleted = newTurnsCompleted; }
	void productionResetTurns() { mTurnsCompleted = 0; }

	ProductType productType() const { return mProduct; }
	void productType(ProductType type);

	ProductType productWaiting() const { return mProductWaiting; }
	ProductType pullProduct();

	const ProductionTypeList& productList() const { return mAvailableProducts; }

	virtual void initFactory() = 0;

	ProductionSignal::Source& productionComplete() { return mProductionComplete; }

	NAS2D::Dictionary getDataDict() const override;

protected:
	void clearProduction();

	void addProduct(ProductType type);
	bool enoughResourcesAvailable();

	const StorableResources* resourcePool() { return mResources; }

private:
	int mTurnsCompleted = 0;
	int mTurnsToComplete = 0;

	ProductType mProduct = ProductType::PRODUCT_NONE;
	ProductType mProductWaiting = ProductType::PRODUCT_NONE; /**< Product that is waiting to be pulled from the factory. */

	ProductionTypeList mAvailableProducts; /**< List of products that the Factory can produce. */

	ProductionSignal mProductionComplete; /**< Signal used when production is complete. */

	const StorableResources* mResources = nullptr; /**< Pointer to the player's resource pool. UGLY. */
};

const ProductionCost& productCost(ProductType);
