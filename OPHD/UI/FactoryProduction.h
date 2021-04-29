#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "Core/CheckBox.h"
#include "IconGrid.h"

#include "../Constants.h"
#include "../Common.h"
#include "../ProductionCost.h"


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

private:
	void onOkay();
	void onCancel();
	void onClearSelection();
	void onCheckBoxIdleChange();
	void onApply();

	void clearProduct();

	void onProductSelectionChange(const IconGrid::IconGridItem*);

	Factory* mFactory = nullptr;

	ProductType mProduct = ProductType::PRODUCT_NONE;
	ProductionCost mProductCost;

	IconGrid mProductGrid{"ui/factory.png", 32, constants::MARGIN_TIGHT};

	Button btnOkay{"Okay"};
	Button btnCancel{"Cancel"};
	Button btnClearSelection{"Clear Selection"};
	Button btnApply{"Apply"};

	CheckBox chkIdle{"Idle"};
};
