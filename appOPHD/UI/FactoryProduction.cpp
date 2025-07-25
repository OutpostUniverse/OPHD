#include "FactoryProduction.h"

#include "StringTable.h"

#include "../Constants/UiConstants.h"
#include "../Resources.h"
#include "../Constants/Strings.h"
#include "../MapObjects/Structures/Factory.h"

#include <libOPHD/ProductCatalog.h>

#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <stdexcept>


using namespace NAS2D;


namespace
{
	StringTable factoryProductionStringTable(const ProductionCost& productionCost, int turnsCompleted)
	{
		const auto totalCost = productionCost.resourceCost * productionCost.turnsToBuild;

		StringTable stringTable(2, 5);
		stringTable.setColumnJustification(1, StringTable::Justification::Right);

		stringTable.setColumnText(
			0,
			{
				"Turns Completed:",
				ResourceNamesRefined[0] + ":",
				ResourceNamesRefined[1] + ":",
				ResourceNamesRefined[2] + ":",
				ResourceNamesRefined[3] + ":",
			}
		);

		stringTable.setColumnText(
			1,
			{
				std::to_string(turnsCompleted) + " of " + std::to_string(productionCost.turnsToBuild),
				std::to_string(totalCost.resources[0]),
				std::to_string(totalCost.resources[1]),
				std::to_string(totalCost.resources[2]),
				std::to_string(totalCost.resources[3]),
			}
		);

		return stringTable;
	}
}

FactoryProduction::FactoryProduction() :
	Window{constants::WindowFactoryProduction},
	mFactory{nullptr},
	mProduct{ProductType::PRODUCT_NONE},
	mProductGrid{{this, &FactoryProduction::onProductSelectionChange}, "ui/factory.png", 32, constants::MarginTight, true},
	chkIdle{"Idle", {this, &FactoryProduction::onCheckBoxIdleChange}},
	btnClearSelection{"Clear Selection", {this, &FactoryProduction::onClearSelection}},
	btnApply{"Apply", {this, &FactoryProduction::onApply}},
	btnOkay{"Okay", {this, &FactoryProduction::onOkay}},
	btnCancel{"Cancel", {this, &FactoryProduction::onCancel}}
{
	mProductGrid.size({140, 110});
	mProductGrid.hide();
	add(mProductGrid, {constants::Margin, sWindowTitleBarHeight + constants::Margin});

	// Create dummy object for sizing purposes
	auto stringTable = factoryProductionStringTable({99, {}}, 99);
	stringTable.position(mProductGrid.area().crossXPoint() + NAS2D::Vector{constants::Margin, 0});
	stringTable.computeRelativeCellPositions();

	chkIdle.size({50, 20});
	add(chkIdle, stringTable.area().crossYPoint() - NAS2D::Point{0, 0} + NAS2D::Vector{0, constants::Margin});

	mProductGrid.height(chkIdle.area().endPoint().y - mProductGrid.area().position.y);

	const auto buttonArea = Rectangle{mProductGrid.area().endPoint() + Vector{constants::Margin, constants::MarginTight}, {std::max(162, stringTable.area().size.x), 22}};
	const auto buttonSize = Vector{(buttonArea.size.x - (constants::MarginTight * 2)) / 3, buttonArea.size.y};
	const auto buttonSpacing = buttonSize.x + constants::MarginTight;

	btnClearSelection.size({mProductGrid.size().x, buttonSize.y});
	add(btnClearSelection, {constants::Margin, buttonArea.position.y});

	btnApply.size(buttonSize);
	add(btnApply, {buttonArea.position.x, buttonArea.position.y});

	btnOkay.size(buttonSize);
	add(btnOkay, {buttonArea.position.x + buttonSpacing, buttonArea.position.y});

	btnCancel.size(buttonSize);
	add(btnCancel, {buttonArea.position.x + buttonSpacing * 2, buttonArea.position.y});

	size(buttonArea.endPoint() - NAS2D::Point{0, 0} + NAS2D::Vector{constants::Margin, constants::Margin});
}


void FactoryProduction::hide()
{
	Control::hide();
	mFactory = nullptr;
	clearProduct();
}


void FactoryProduction::onProductSelectionChange(const IconGridItem* item)
{
	if (!mFactory) { return; }

	if (!item)
	{
		mProductCost = {};
		return;
	}

	mProduct = static_cast<ProductType>(item->meta);
	mProductCost = productCost(mProduct);
}


void FactoryProduction::onClearSelection()
{
	clearProduct();
	onApply();
}


void FactoryProduction::onApply()
{
	mFactory->productType(mProduct);
}


void FactoryProduction::onOkay()
{
	mFactory->productType(mProduct);
	hide();
}


void FactoryProduction::onCancel()
{
	hide();
}


void FactoryProduction::onCheckBoxIdleChange()
{
	if (!mFactory) { return; }

	mFactory->forceIdle(chkIdle.checked());
}


void FactoryProduction::clearProduct()
{
	mProduct = ProductType::PRODUCT_NONE;
	mProductCost = {};
	mProductGrid.clearSelection();
}


void FactoryProduction::factory(Factory* newFactory)
{
	mFactory = newFactory;

	if (!mFactory) { return; }

	mProductGrid.clear();
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
		mProductGrid.addItem({ProductCatalog::get(ptlist[i]).name, ptlist[i], ptlist[i]});
	}

	if (mFactory->productType() == ProductType::PRODUCT_NONE) { mProductGrid.clearSelection(); }
	else { mProductGrid.setSelectionByMeta(static_cast<int>(mFactory->productType())); }

	mProduct = mFactory->productType();
	mProductCost = productCost(mFactory->productType());
}


void FactoryProduction::drawClientArea(NAS2D::Renderer& renderer) const
{
	auto stringTable = factoryProductionStringTable(mProductCost, mFactory->productionTurnsCompleted());
	stringTable.position(mProductGrid.area().crossXPoint() + NAS2D::Vector{constants::Margin, 0});
	stringTable.computeRelativeCellPositions();
	stringTable.draw(renderer);
}
