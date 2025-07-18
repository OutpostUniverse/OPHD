#pragma once

#include "IconGrid.h"

#include <libOPHD/EnumProductType.h>
#include <libOPHD/ProductionCost.h>

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

	void drawClientArea(NAS2D::Renderer& renderer) const override;

protected:
	void onProductSelectionChange(const IconGridItem*);
	void onClearSelection();
	void onApply();
	void onOkay();
	void onCancel();
	void onCheckBoxIdleChange();

	void clearProduct();

private:
	Factory* mFactory;

	ProductType mProduct;
	ProductionCost mProductCost;

	IconGrid mProductGrid;

	CheckBox chkIdle;

	Button btnClearSelection;
	Button btnApply;
	Button btnOkay;
	Button btnCancel;
};
