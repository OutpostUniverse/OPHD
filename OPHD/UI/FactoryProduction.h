#pragma once

#include "UI.h"
#include "IconGrid.h"

#include "../Things/Structures/Factory.h"

/**
 * \brief Implements a Factory Production dialog interface.
 */
class FactoryProduction : public Window
{
public:
	FactoryProduction();
	~FactoryProduction() override;

	void factory(Factory* newFactory);
	Factory* factory() { return mFactory; }

	void hide() override;
	
	void update() override;

protected:
	void init();

private:
	void btnOkayClicked();
	void btnCancelClicked();
	void btnClearSelectionClicked();
	void chkIdleClicked();
	void btnApplyClicked();

	void clearProduct();

	void productSelectionChanged(const IconGrid::IconGridItem*);

private:
	FactoryProduction(const FactoryProduction&) = delete;
	FactoryProduction& operator=(const FactoryProduction&) = delete;

private:
	Factory* mFactory = nullptr;

	ProductType mProduct = ProductType::PRODUCT_NONE;
	ProductionCost mProductCost;

	IconGrid mProductGrid;

	Button btnOkay;
	Button btnCancel;
	Button btnClearSelection;
	Button btnApply;

	CheckBox chkIdle;
};
