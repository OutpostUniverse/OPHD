#pragma once

#include "IconGrid.h"

#include "../ProductionCost.h"

#include <libOPHD/EnumProductType.h>

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/CheckBox.h>


class Factory;


/**
 * Implements a Factory Production dialog interface.
 */
class FactoryProduction : public Window
{
public:
	FactoryProduction();

	void factory(Factory* newFactory);
	Factory* factory() { return mFactory; }

	void hide() override;

	void update() override;

protected:
	void onOkay();
	void onCancel();
	void onClearSelection();
	void onCheckBoxIdleChange();
	void onApply();

	void clearProduct();

	void onProductSelectionChange(const IconGrid::Item*);

private:
	Factory* mFactory;

	ProductType mProduct;
	ProductionCost mProductCost;

	IconGrid mProductGrid;

	Button btnOkay;
	Button btnCancel;
	Button btnClearSelection;
	Button btnApply;

	CheckBox chkIdle;
};
