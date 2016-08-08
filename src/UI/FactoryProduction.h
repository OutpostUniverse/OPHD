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

	void productionSelectionChanged(const std::string&);

private:
	Factory*					mFactory;

	Factory::ProductionType		mProductionType;
	ProductionCost				mProductionCost;

	Font						mBold;

	IconGrid					mProductionGrid;

	Button						btnOkay;
	Button						btnCancel;
};