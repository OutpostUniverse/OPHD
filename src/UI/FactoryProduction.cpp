#include "FactoryProduction.h"

#include "../Constants.h"


std::map<std::string, Factory::ProductionType> PRODUCTION_TRANSLATION_TABLE;

FactoryProduction::FactoryProduction(Font& font) : mFactory(nullptr), mProductionType(Factory::PRODUCTION_NONE), mBold("fonts/ui-bold.png", 7, 9, 0)
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
	addControl("mProductionGrid", &mProductionGrid, constants::MARGIN, 25);
	mProductionGrid.font(font());
	mProductionGrid.sheetPath("ui/surface_factory.png");
	mProductionGrid.size(140, 110);
	mProductionGrid.iconSize(32);
	mProductionGrid.iconMargin(constants::MARGIN_TIGHT);
	mProductionGrid.showTooltip(true);
	mProductionGrid.hide();
	mProductionGrid.selectionChanged().Connect(this, &FactoryProduction::productionSelectionChanged);

	addControl("btnOkay", &btnOkay, 63, 136);
	btnOkay.font(font());
	btnOkay.text("Okay");
	btnOkay.size(40, 20);
	btnOkay.click().Connect(this, &FactoryProduction::btnOkayClicked);

	addControl("btnCancel", &btnCancel, 106, 136);
	btnCancel.font(font());
	btnCancel.text("Cancel");
	btnCancel.size(40, 20);
	btnCancel.click().Connect(this, &FactoryProduction::btnCancelClicked);

	// Fill production translation table
	PRODUCTION_TRANSLATION_TABLE[""] = Factory::PRODUCTION_NONE;
	PRODUCTION_TRANSLATION_TABLE[constants::ROBODIGGER] = Factory::PRODUCTION_DIGGER;
	PRODUCTION_TRANSLATION_TABLE[constants::ROBODOZER] = Factory::PRODUCTION_DOZER;
	PRODUCTION_TRANSLATION_TABLE[constants::ROBOMINER] = Factory::PRODUCTION_MINER;
}


void FactoryProduction::hide()
{
	Control::hide();
	mFactory = nullptr;
	mProductionType = Factory::PRODUCTION_NONE;
	mProductionGrid.clearSelection();
}


void FactoryProduction::productionSelectionChanged(const std::string& _s)
{
	if (!mFactory)
		return;

	if (PRODUCTION_TRANSLATION_TABLE.find(_s) == PRODUCTION_TRANSLATION_TABLE.end())
		throw Exception(0, "Bad Production Code", "FactoryProduction::productionSelectionChanged() called with an undefined production code: " + _s);

	mProductionType = PRODUCTION_TRANSLATION_TABLE[_s];

	if (_s.empty())
	{
		mProductionCost.clear();
		return;
	}
	
	mProductionCost = mFactory->productionCost(mProductionType);
}


void FactoryProduction::btnOkayClicked()
{
	mFactory->productionType(mProductionType);
	hide();
}


void FactoryProduction::btnCancelClicked()
{
	hide();
}


void FactoryProduction::factory(Factory* _f)
{
	mFactory = _f;

	if (mFactory == nullptr)
		return;

	mProductionGrid.dropAllItems();

	Factory::ProductionTypeList ptlist = mFactory->productionList();
	
	// FIXME: Very seriously doubt that this check is needed.
	if (ptlist.empty())
		return;

	// FIXME: This is a super naive and ugly way to do this. Don't like it at all.
	for (size_t i = 0; i < ptlist.size(); ++i)
	{
		if(ptlist[i] == Factory::PRODUCTION_DIGGER)
			mProductionGrid.addItem(constants::ROBODIGGER, 0);
		else if (ptlist[i] == Factory::PRODUCTION_DOZER)
			mProductionGrid.addItem(constants::ROBODOZER, 1);
		else if (ptlist[i] == Factory::PRODUCTION_MINER)
			mProductionGrid.addItem(constants::ROBOMINER, 2);
	}

	// FIXME:	The following block assumes that factory items will be in a particular order and
	//			is prone to break if things aren't supplied in exactly the way it expects. Also,
	//			it's just another really bad way of doing this (if/else if blocks, blech!)
	if (mFactory->productionType() == Factory::PRODUCTION_DIGGER)
		mProductionGrid.selection(0);
	else if (mFactory->productionType() == Factory::PRODUCTION_DOZER)
		mProductionGrid.selection(1);
	else if (mFactory->productionType() == Factory::PRODUCTION_MINER)
		mProductionGrid.selection(2);
	else
		mProductionGrid.clearSelection();

	mProductionCost = mFactory->productionCost(mFactory->productionType());
}


void FactoryProduction::update()
{
	if (!visible())
		return;

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawText(mBold, "Turns Completed:", rect().x() + constants::MARGIN * 2 + mProductionGrid.width(), rect().y() + 25.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i of %i", mFactory->productionTurnsCompleted(), mProductionCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductionGrid.width() + 120, rect().y() + 25.0f, 255, 255, 255);

	r.drawText(mBold, "Common Metals:", rect().x() + constants::MARGIN * 2 + mProductionGrid.width(), rect().y() + 45.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductionCost.CostPerTurn.commonMetals() * mProductionCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductionGrid.width() + 120, rect().y() + 45.0f, 255, 255, 255);

	r.drawText(mBold, "Common Minerals:", rect().x() + constants::MARGIN * 2 + mProductionGrid.width(), rect().y() + 55.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductionCost.CostPerTurn.commonMinerals() * mProductionCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductionGrid.width() + 120, rect().y() + 55.0f, 255, 255, 255);

	r.drawText(mBold, "Rare Metals:", rect().x() + constants::MARGIN * 2 + mProductionGrid.width(), rect().y() + 65.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductionCost.CostPerTurn.rareMetals() * mProductionCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductionGrid.width() + 120, rect().y() + 65.0f, 255, 255, 255);

	r.drawText(mBold, "Rare Minerals:", rect().x() + constants::MARGIN * 2 + mProductionGrid.width(), rect().y() + 75.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i", mProductionCost.CostPerTurn.rareMinerals() * mProductionCost.TurnsToBuild), rect().x() + constants::MARGIN * 2 + mProductionGrid.width() + 120, rect().y() + 75.0f, 255, 255, 255);
}
