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

	virtual void update();

	virtual void hide();

protected:
	virtual void init();

private:
	void btnOkayClicked();
	void btnCancelClicked();
	void btnClearSelectionClicked();
	void btnIdleClicked();

	void clearProduct();

	FactoryProduction();
	FactoryProduction(const FactoryProduction&);
	FactoryProduction& operator=(const FactoryProduction&) = delete;

	void productSelectionChanged(const IconGrid::IconGridItem*);

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
};
