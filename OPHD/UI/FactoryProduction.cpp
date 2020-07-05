// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryProduction.h"

#include "StringTable.h"
#include "../Constants.h"

using namespace NAS2D;


/**
 * 
 */
FactoryProduction::FactoryProduction() :
	btnOkay{"Okay"},
	btnCancel{"Cancel"},
	btnClearSelection{"Clear Selection"},
	btnApply{"Apply"},
	chkIdle{"Idle"}
{
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
	size({320, 162});

	// Set up GUI Layout
	add(&mProductGrid, static_cast<float>(constants::MARGIN), 25);
	mProductGrid.sheetPath("ui/factory.png");
	mProductGrid.size({140, 110});
	mProductGrid.iconSize(32);
	mProductGrid.iconMargin(constants::MARGIN_TIGHT);
	mProductGrid.showTooltip(true);
	mProductGrid.hide();
	mProductGrid.selectionChanged().connect(this, &FactoryProduction::productSelectionChanged);

	add(&btnOkay, 233, 138);
	btnOkay.size({40, 20});
	btnOkay.click().connect(this, &FactoryProduction::btnOkayClicked);

	add(&btnCancel, 276, 138);
	btnCancel.size({40, 20});
	btnCancel.click().connect(this, &FactoryProduction::btnCancelClicked);

	add(&btnClearSelection, 5, 138);
	btnClearSelection.size({mProductGrid.width(), 20});
	btnClearSelection.click().connect(this, &FactoryProduction::btnClearSelectionClicked);

	add(&btnApply, mProductGrid.width() + 12, btnClearSelection.positionY());
	btnApply.size({40, 20});
	btnApply.click().connect(this, &FactoryProduction::btnApplyClicked);

	add(&chkIdle, mProductGrid.width() + 12, 115);
	chkIdle.size({50, 20});
	chkIdle.click().connect(this, &FactoryProduction::chkIdleClicked);
}


/**
 * 
 */
void FactoryProduction::clearProduct()
{
	mProduct = ProductType::PRODUCT_NONE;
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
	mProductCost = productCost(mProduct);
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
void FactoryProduction::chkIdleClicked()
{
	if (!mFactory) { return; }
	
	mFactory->forceIdle(chkIdle.checked());
}


/**
 * 
 */
void FactoryProduction::factory(Factory* newFactory)
{
	mFactory = newFactory;

	if (!mFactory) { return; }

	mProductGrid.dropAllItems();
	clearProduct();

	// destroyed factories can't produce anything at all ever.
	if (mFactory->destroyed())
	{
		chkIdle.enabled(false);
		chkIdle.checked(false);
		return;
	}

	chkIdle.checked(mFactory->isIdle());

	const Factory::ProductionTypeList& ptlist = mFactory->productList();

	// The production type list from the factory should never, ever be empty. If it is there is
	// a clear logic error somewhere so fail very loudly and conspicuously here.
	if (ptlist.empty())
	{
		throw std::runtime_error("FactoryProduction::factory(): Factory provided with an empty production type list.");
	}

	for (std::size_t i = 0; i < ptlist.size(); ++i)
	{
		mProductGrid.addItem(productDescription(ptlist[i]), ptlist[i], ptlist[i]);
	}

	if (mFactory->productType() == ProductType::PRODUCT_NONE) { mProductGrid.clearSelection(); }
	else { mProductGrid.selection_meta(static_cast<int>(mFactory->productType())); }

	mProduct = mFactory->productType();
	mProductCost = productCost(mFactory->productType());
}


/**
 * 
 */
void FactoryProduction::update()
{
	if (!visible()) { return; }

	Window::update();

	StringTable stringTable(2, 5);
	stringTable.setPosition(mRect.startPoint() + NAS2D::Vector<float>{ constants::MARGIN * 2 + mProductGrid.width(), 25 });
	stringTable.setColumnJustification(1, StringTable::Justification::Right);

	stringTable.at(0, 0).text = "Turns Completed:";
	stringTable.at(1, 0).text = std::to_string(mFactory->productionTurnsCompleted()) + " of " + std::to_string(mProductCost.turnsToBuild());

	stringTable.at(0, 1).text = "Common Metals:";
	stringTable.at(1, 1).text = std::to_string(mProductCost.commonMetals() * mProductCost.turnsToBuild());

	stringTable.at(0, 2).text = "Common Minerals:";
	stringTable.at(1, 2).text = std::to_string(mProductCost.commonMinerals() * mProductCost.turnsToBuild());

	stringTable.at(0, 3).text = "Rare Metals:";
	stringTable.at(1, 3).text = std::to_string(mProductCost.rareMetals() * mProductCost.turnsToBuild());

	stringTable.at(0, 4).text = "Rare Minerals:";
	stringTable.at(1, 4).text = std::to_string(mProductCost.rareMinerals() * mProductCost.turnsToBuild());

	stringTable.computeRelativeCellPositions();
	stringTable.draw(Utility<Renderer>::get());
}
