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

	FactoryProduction();
	FactoryProduction(const FactoryProduction&);
	FactoryProduction& operator=(const FactoryProduction&);

	void productSelectionChanged(const std::string&);

private:
	Factory*					mFactory;

	Factory::ProductType		mProduct;
	ProductionCost				mProductCost;

	Font						mBold;

	IconGrid					mProductGrid;

	Button						btnOkay;
	Button						btnCancel;
};