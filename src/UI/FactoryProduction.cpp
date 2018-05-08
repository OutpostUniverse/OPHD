// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryProduction.h"

#include "../Constants.h"


#include <array>


/**
 * Description table for products.
 */
std::array<std::string, PRODUCT_COUNT> PRODUCT_DESCRIPTION_TABLE;


/**
 * 
 */
FactoryProduction::FactoryProduction(Font& font) : mBold("fonts/opensans-bold.ttf", 10)
{
	Control::font(font);
	text(constants::WINDOW_FACTORY_PRODUCTION);
	init();
}


/**
 * D'tor
 */
FactoryProduction::~FactoryProduction()
{}


/**
 * 
 */
void FactoryProduction::init()
{
	size(320, 162);

	// Set up GUI Layout
	addControl("mProductionGrid", &mProductGrid, static_cast<float>(constants::MARGIN), 25);
	mProductGrid.font(font());
	mProductGrid.sheetPath("ui/factory.png");
	mProductGrid.size(140, 110);
	mProductGrid.iconSize(32);
	mProductGrid.iconMargin(constants::MARGIN_TIGHT);
	mProductGrid.showTooltip(true);
	mProductGrid.hide();
	mProductGrid.selectionChanged().connect(this, &FactoryProduction::productSelectionChanged);

	addControl("btnOkay", &btnOkay, 233, 138);
	btnOkay.font(font());
	btnOkay.text("Okay");
	btnOkay.size(40, 20);
	btnOkay.click().connect(this, &FactoryProduction::btnOkayClicked);

	addControl("btnCancel", &btnCancel, 276, 138);
	btnCancel.font(font());
	btnCancel.text("Cancel");
	btnCancel.size(40, 20);
	btnCancel.click().connect(this, &FactoryProduction::btnCancelClicked);

	addControl("btnClearSelection", &btnClearSelection, 5, 138);
	btnClearSelection.font(font());
	btnClearSelection.text("Clear Selection");
	btnClearSelection.size(90, 20);
	btnClearSelection.click().connect(this, &FactoryProduction::btnClearSelectionClicked);

	addControl("btnIdle", &btnIdle, mProductGrid.positionX() + mProductGrid.width() - 40, 138);
	btnIdle.font(font());
	btnIdle.text("Idle");
	btnIdle.size(40, 20);
	btnIdle.click().connect(this, &FactoryProduction::btnIdleClicked);
	btnIdle.type(Button::BUTTON_TOGGLE);

	addControl("btnApply", &btnApply, btnIdle.positionX() + btnIdle.width() + 10, btnIdle.positionY());
	btnApply.font(font());
	btnApply.text("Apply");
	btnApply.size(40, 20);
	btnApply.click().connect(this, &FactoryProduction::btnApplyClicked);


	// Fill product description table
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_DIGGER] = constants::ROBODIGGER;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_DOZER] = constants::ROBODOZER;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_EXPLORER] = constants::ROBOEXPLORER;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_MINER] = constants::ROBOMINER;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_ROAD_MATERIALS] = constants::ROAD_MATERIALS;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_TRUCK] = constants::TRUCK;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_MAINTENANCE_PARTS] = constants::MAINTENANCE_SUPPLIES;

	PRODUCT_DESCRIPTION_TABLE[PRODUCT_CLOTHING] = constants::CLOTHING;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_MEDICINE] = constants::MEDICINE;
}


/**
 * 
 */
void FactoryProduction::clearProduct()
{
	mProduct = PRODUCT_NONE;
	mProductCost.clear();
	mProductGrid.clearSelection();
}


/**
 * 
 */
void FactoryProduction::hide()
{
	Control::hide();
	mFactory = nullptr;
	clearProduct();
}


/**
 * 
 */
void FactoryProduction::productSelectionChanged(const IconGrid::IconGridItem* _item)
{
	if (!mFactory) { return; }

	if (!_item)
	{
		mProductCost.clear();
		return;
	}

	mProduct = static_cast<ProductType>(_item->meta);
	mProductCost = mFactory->productCost(mProduct);
}


/**
 * 
 */
void FactoryProduction::btnOkayClicked()
{
	mFactory->productType(mProduct);
	hide();
}


/**
 * 
 */
void FactoryProduction::btnApplyClicked()
{
	mFactory->productType(mProduct);
}


/**
 * 
 */
void FactoryProduction::btnCancelClicked()
{
	hide();
}


/**
 * 
 */
void FactoryProduction::btnClearSelectionClicked()
{
	clearProduct();
	btnApplyClicked();
}


/**
 * 
 */
void FactoryProduction::btnIdleClicked()
{
	if (!mFactory) { return; }
	
	mFactory->forceIdle(btnIdle.toggled());
}


/**
 * 
 */
void FactoryProduction::factory(Factory* _f)
{
	mFactory = _f;

	if (mFactory == nullptr) { return; }

	mProductGrid.dropAllItems();
	clearProduct();

	// destroyed factories can't produce anything at all ever.
	if (mFactory->destroyed())
	{
		btnIdle.enabled(false);
		return;
	}

	btnIdle.toggle(mFactory->isIdle());

	const Factory::ProductionTypeList& ptlist = mFactory->productList();

	// The production type list from the factory should never, ever be empty. If it is there is
	// a clear logic error somewhere so fail very loudly and conspicuously here.
	if (ptlist.empty())
	{
		throw std::runtime_error("FactoryProduction::factory(): Factory provided with an empty production type list.");
	}

	for (size_t i = 0; i < ptlist.size(); ++i)
	{
		mProductGrid.addItem(PRODUCT_DESCRIPTION_TABLE[ptlist[i]], ptlist[i], ptlist[i]);
	}

	if (mFactory->productType() == PRODUCT_NONE) { mProductGrid.clearSelection(); }
	else { mProductGrid.selection_meta(static_cast<int>(mFactory->productType())); }

	mProductCost = mFactory->productCost(mFactory->productType());
}


/**
 * 
 */
void FactoryProduction::update()
{
	if (!visible()) { return; }

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawText(mBold, "Turns Completed:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 25.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i of %i", mFactory->productionTurnsCompleted(), mProductCost.turnsToBuild()), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 25.0f, 255, 255, 255);

	r.drawText(mBold, "Common Metals:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 45.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductCost.commonMetals() * mProductCost.turnsToBuild()), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 45.0f, 255, 255, 255);

	r.drawText(mBold, "Common Minerals:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 55.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductCost.commonMinerals() * mProductCost.turnsToBuild()), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 55.0f, 255, 255, 255);

	r.drawText(mBold, "Rare Metals:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 65.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductCost.rareMetals() * mProductCost.turnsToBuild()), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 65.0f, 255, 255, 255);

	r.drawText(mBold, "Rare Minerals:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 75.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductCost.rareMinerals() * mProductCost.turnsToBuild()), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 75.0f, 255, 255, 255);
}
