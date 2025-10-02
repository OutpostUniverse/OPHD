#pragma once

#include "../Structure.h"

#include <libOPHD/EnumProductType.h>

#include <NAS2D/Signal/Delegate.h>

#include <vector>


struct ProductionCost;
struct StorableResources;


/**
 * Defines the Factory interface.
 *
 * Factory derives from Structure and provides the basic factory interface and
 * the underlying factory production code. Exactly what a factory is capable of
 * producing is up to the derived factory type.
 *
 * \note	The Factory interface defines two public functions: resourcePool() and robotPool().
 *			These must be called manually whenever a derived factory type is added to the game as
 *			there are no manager objects that do this (by design).
 *
 * \warning	There are no sanity checks in the underlying production code to check if resourcePool
 *			or robotPool have been properly set. It is assumed that they have been.
 */
class Factory : public Structure
{
public:
	using ProductionCompleteDelegate = NAS2D::Delegate<void(Factory&)>;

	using ProductionTypeList = std::vector<ProductType>;

public:
	Factory(StructureID id, std::vector<ProductType> products, Tile& tile);

	virtual void updateProduction();

	void resourcePool(const StorableResources* resources);

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

	void productionCompleteHandler(ProductionCompleteDelegate newProductionCompleteHandler);

	NAS2D::Dictionary getDataDict() const override;

protected:
	void clearProduction();

	void addProduct(ProductType type);
	bool enoughResourcesAvailable();

private:
	int mTurnsCompleted = 0;
	int mTurnsToComplete = 0;

	ProductType mProduct;
	ProductType mProductWaiting; /**< Product that is waiting to be pulled from the factory. */

	ProductionTypeList mAvailableProducts; /**< List of products that the Factory can produce. */

	ProductionCompleteDelegate mProductionCompleteHandler; /**< Delegate used when production is complete. */

	const StorableResources* mResources = nullptr; /**< Pointer to the player's resource pool. UGLY. */
};

const ProductionCost& productCost(ProductType);
