#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "Core/CheckBox.h"
#include "IconGrid.h"

#include "../Constants.h"
#include "../Things/Structures/Factory.h"


/**
 * \brief Implements a Factory Production dialog interface.
 */
class FactoryProduction : public Window
{
public:
	FactoryProduction();
	FactoryProduction(const FactoryProduction&) = delete;
	FactoryProduction& operator=(const FactoryProduction&) = delete;
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
