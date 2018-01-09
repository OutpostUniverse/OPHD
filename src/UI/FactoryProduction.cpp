#include "FactoryProduction.h"

#include "../Constants.h"


#include <array>


std::map<std::string, ProductType> PRODUCTION_TRANSLATION_TABLE;
std::array<std::string, PRODUCT_COUNT> PRODUCT_DESCRIPTION_TABLE;


FactoryProduction::FactoryProduction(Font& font) : mBold("fonts/opensans-bold.ttf", 10)
{
	Control::font(font);
	text(constants::WINDOW_FACTORY_PRODUCTION);
	init();
}


FactoryProduction::~FactoryProduction()
{}


void FactoryProduction::init()
{
	size(320, 162);

	// Set up GUI Layout
	addControl("mProductionGrid", &mProductGrid, static_cast<float>(constants::MARGIN), 25);
	mProductGrid.font(font());
	mProductGrid.sheetPath("ui/surface_factory.png");
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


	// Fill production translation table
	PRODUCTION_TRANSLATION_TABLE[""] = PRODUCT_NONE;
	PRODUCTION_TRANSLATION_TABLE[constants::ROBODIGGER] = PRODUCT_DIGGER;
	PRODUCTION_TRANSLATION_TABLE[constants::ROBODOZER] = PRODUCT_DOZER;
	PRODUCTION_TRANSLATION_TABLE[constants::ROBOMINER] = PRODUCT_MINER;

	// Fill product description table
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_DIGGER] = constants::ROBODIGGER;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_DOZER] = constants::ROBODOZER;
	PRODUCT_DESCRIPTION_TABLE[PRODUCT_MINER] = constants::ROBOMINER;
}


void FactoryProduction::clearProduct()
{
	mProduct = PRODUCT_NONE;
	mProductCost.clear();
	mProductGrid.clearSelection();
}


void FactoryProduction::hide()
{
	Control::hide();
	mFactory = nullptr;
	clearProduct();
}


void FactoryProduction::productSelectionChanged(const std::string& _s)
{
	if (!mFactory) { return; }
	if (PRODUCTION_TRANSLATION_TABLE.find(_s) == PRODUCTION_TRANSLATION_TABLE.end())
	{
		throw std::runtime_error("FactoryProduction::productionSelectionChanged() called with an undefined production code: " + _s);
	}

	mProduct = PRODUCTION_TRANSLATION_TABLE[_s];

	if (_s.empty())
	{
		mProductCost.clear();
		return;
	}
	
	mProductCost = mFactory->productCost(mProduct);
}


void FactoryProduction::btnOkayClicked()
{
	mFactory->productType(mProduct);
	hide();
}


void FactoryProduction::btnCancelClicked()
{
	hide();
}


void FactoryProduction::btnClearSelectionClicked()
{
	clearProduct();
}


void FactoryProduction::btnIdleClicked()
{
	if (!mFactory) { return; }
	
	mFactory->forceIdle(btnIdle.toggled());
}


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

	// FIXME: Very seriously doubt that this check is needed.
	if (ptlist.empty()) { return; }

	for (size_t i = 0; i < ptlist.size(); ++i) { mProductGrid.addItem(PRODUCT_DESCRIPTION_TABLE[ptlist[i]], i); }

	if (mFactory->productType() == PRODUCT_NONE) { mProductGrid.clearSelection(); }
	else { mProductGrid.selection(static_cast<int>(mFactory->productType())); }

	mProductCost = mFactory->productCost(mFactory->productType());
}


void FactoryProduction::update()
{
	if (!visible()) { return; }

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawText(mBold, "Turns Completed:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 25.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i of %i", mFactory->productionTurnsCompleted(), mProductCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 25.0f, 255, 255, 255);

	r.drawText(mBold, "Common Metals:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 45.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductCost.CostPerTurn.commonMetals() * mProductCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 45.0f, 255, 255, 255);

	r.drawText(mBold, "Common Minerals:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 55.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductCost.CostPerTurn.commonMinerals() * mProductCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 55.0f, 255, 255, 255);

	r.drawText(mBold, "Rare Metals:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 65.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductCost.CostPerTurn.rareMetals() * mProductCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 65.0f, 255, 255, 255);

	r.drawText(mBold, "Rare Minerals:", rect().x() + constants::MARGIN * 2 + mProductGrid.width(), rect().y() + 75.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductCost.CostPerTurn.rareMinerals() * mProductCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductGrid.width() + 120, rect().y() + 75.0f, 255, 255, 255);
}
