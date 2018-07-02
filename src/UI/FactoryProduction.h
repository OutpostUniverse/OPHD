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
	FactoryProduction(Font& font);
	virtual ~FactoryProduction();

	void factory(Factory* _f);
	Factory* factory() { return mFactory; }

	virtual void update();
	virtual void hide();

protected:
	virtual void init();

private:
	void btnOkayClicked();
	void btnCancelClicked();
	void btnClearSelectionClicked();
	void btnIdleClicked();
	void btnApplyClicked();

	void clearProduct();

	void productSelectionChanged(const IconGrid::IconGridItem*);

private:
	FactoryProduction() = delete;
	FactoryProduction(const FactoryProduction&) = delete;
	FactoryProduction& operator=(const FactoryProduction&) = delete;

private:
	Factory*			mFactory = nullptr;

	ProductType			mProduct = PRODUCT_NONE;
	ProductionCost		mProductCost;

	Font				mBold;

	IconGrid			mProductGrid;

	Button				btnOkay;
	Button				btnCancel;
	Button				btnClearSelection;
	Button				btnIdle;
	Button				btnApply;
};
