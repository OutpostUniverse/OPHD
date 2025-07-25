#include "WarehouseReport.h"

#include "../ProgressBar.h"

#include "../../Cache.h"
#include "../../StructureManager.h"
#include "../../Constants/Strings.h"
#include "../../Constants/UiConstants.h"
#include "../../MapObjects/Structures/Warehouse.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Renderer/Renderer.h>

#include <string>
#include <iterator>
#include <algorithm>


class Structure;


using namespace NAS2D;


namespace
{
	constexpr auto filterButtonSectionOffset = NAS2D::Vector{10, 10};
	constexpr auto filterButtonSize = NAS2D::Vector{94, 26};
	constexpr auto infoSectionOffset = filterButtonSectionOffset + NAS2D::Vector{0, filterButtonSize.y + 10};
	constexpr auto infoSectionHeight = 66;
	constexpr auto structureListBoxOffset = infoSectionOffset + NAS2D::Vector{0, infoSectionHeight + 10};


	template <typename Predicate>
	std::vector<Warehouse*> selectWarehouses(const Predicate& predicate)
	{
		const auto& warehouses = Utility<StructureManager>::get().getStructures<Warehouse>();

		std::vector<Warehouse*> output;
		std::copy_if(warehouses.begin(), warehouses.end(), std::back_inserter(output), predicate);

		return output;
	}


	std::string structureStateDescription(const Warehouse& warehouse)
	{
		const auto& productPool = warehouse.products();

		if (!warehouse.operational()) { return warehouse.stateDescription(); }
		else if (productPool.empty()) { return constants::WarehouseEmpty; }
		else if (productPool.atCapacity()) { return constants::WarehouseFull; }
		else if (!productPool.empty() && !productPool.atCapacity()) { return constants::WarehouseVacancy; }

		return std::string{};
	}
}


WarehouseReport::WarehouseReport(TakeMeThereDelegate takeMeThereHandler) :
	mTakeMeThereHandler{takeMeThereHandler},
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	imageWarehouse{imageCache.load("ui/interface/warehouse.png")},
	btnShowAll{"All", {this, &WarehouseReport::onShowAll}},
	btnFull{"Full", {this, &WarehouseReport::onFull}},
	btnVacancy{"Vacancy", {this, &WarehouseReport::onVacancy}},
	btnEmpty{"Empty", {this, &WarehouseReport::onEmpty}},
	btnDisabled{"Disabled", {this, &WarehouseReport::onDisabled}},
	btnTakeMeThere{constants::TakeMeThere, {this, &WarehouseReport::onTakeMeThere}},
	lstStructures{{this, &WarehouseReport::onStructureSelectionChange}}
{
	auto buttonOffset = filterButtonSectionOffset;
	for (auto button : {&btnShowAll, &btnFull, &btnVacancy, &btnEmpty, &btnDisabled})
	{
		button->size(filterButtonSize);
		button->type(Button::Type::Toggle);
		button->toggle(false);
		add(*button, buttonOffset);
		buttonOffset.x += button->size().x + constants::MarginTight;
	}

	btnShowAll.toggle(true);

	btnTakeMeThere.size({140, 30});

	Utility<EventHandler>::get().mouseDoubleClick().connect({this, &WarehouseReport::onDoubleClick});

	fillLists();

	add(btnTakeMeThere, {10, 10});
	add(lstStructures, structureListBoxOffset);
	add(lstProducts, {Utility<Renderer>::get().center().x + 10, 173});
}


WarehouseReport::~WarehouseReport()
{
	Utility<EventHandler>::get().mouseDoubleClick().disconnect({this, &WarehouseReport::onDoubleClick});
}


bool WarehouseReport::canView(const Structure& structure)
{
	return dynamic_cast<const Warehouse*>(&structure);
}


void WarehouseReport::selectStructure(Structure& structure)
{
	lstStructures.setSelected(&structure);
}


void WarehouseReport::clearSelected()
{
	lstStructures.clearSelected();
}


/**
 * Inherited interface. A better name for this function would be
 * fillListWithAll() or something to that effect.
 */
void WarehouseReport::fillLists()
{
	fillListFromStructureList(selectWarehouses([](Warehouse*) { return true; }));
}


void WarehouseReport::refresh()
{
	onShowAll();
}


const Warehouse* WarehouseReport::selectedWarehouse() const
{
	return dynamic_cast<const Warehouse*>(lstStructures.selectedStructure());
}


void WarehouseReport::computeTotalWarehouseCapacity()
{
	int capacityTotal = 0;
	int capacityAvailable = 0;

	const auto& warehouses = Utility<StructureManager>::get().getStructures<Warehouse>();
	for (const auto* warehouse : warehouses)
	{
		if (warehouse->operational())
		{
			const auto& warehouseProducts = warehouse->products();
			capacityAvailable += warehouseProducts.availableStorage();
			capacityTotal += warehouseProducts.capacity();
		}
	}

	warehouseCount = warehouses.size();
	warehouseCapacityTotal = capacityTotal;
	warehouseCapacityUsed = capacityTotal - capacityAvailable;
}


void WarehouseReport::fillListFromStructureList(const std::vector<Warehouse*>& warehouses)
{
	lstStructures.clear();

	for (auto* warehouse : warehouses)
	{
		lstStructures.addItem(warehouse, structureStateDescription(*warehouse));
	}

	if (!lstStructures.isEmpty()) { lstStructures.selectedIndex(0); }
	computeTotalWarehouseCapacity();
}


void WarehouseReport::fillListFull()
{
	const auto predicate = [](Warehouse* wh) {
		return wh->products().atCapacity() && (wh->operational() || wh->isIdle());
	};

	fillListFromStructureList(selectWarehouses(predicate));
}


void WarehouseReport::fillListVacancy()
{
	const auto predicate = [](Warehouse* wh) {
		return !wh->products().atCapacity() && !wh->products().empty() && (wh->operational() || wh->isIdle());
	};

	fillListFromStructureList(selectWarehouses(predicate));
}



void WarehouseReport::fillListEmpty()
{
	const auto predicate = [](Warehouse* wh) {
		return wh->products().empty() && (wh->operational() || wh->isIdle());
	};

	fillListFromStructureList(selectWarehouses(predicate));
}


void WarehouseReport::fillListDisabled()
{
	const auto predicate = [](Warehouse* structure) {
		return structure->disabled() || structure->destroyed();
	};

	fillListFromStructureList(selectWarehouses(predicate));
}


void WarehouseReport::onDoubleClick(MouseButton button, NAS2D::Point<int> position)
{
	if (!visible()) { return; }
	if (button != MouseButton::Left) { return; }

	if (lstStructures.area().contains(position) && selectedWarehouse())
	{
		if (mTakeMeThereHandler) { mTakeMeThereHandler(selectedWarehouse()); }
	}
}


void WarehouseReport::onResize()
{
	Control::onResize();

	lstStructures.size({(mRect.size.x / 2) - 20, mRect.position.y + mRect.size.y - lstStructures.position().y - 10});
	lstProducts.area({
		{Utility<Renderer>::get().center().x + 10, lstProducts.position().y},
		{(mRect.size.x / 2) - 20, mRect.size.y - 184}
	});

	btnTakeMeThere.position({Utility<Renderer>::get().size().x - 150, position().y + 35});
}


void WarehouseReport::onVisibilityChange(bool visible)
{
	Report::onVisibilityChange(visible);

	setVisibility();
}


void WarehouseReport::setVisibility()
{
	const auto isWarehouseSelected = lstStructures.isItemSelected();
	btnTakeMeThere.visible(isWarehouseSelected);
	lstProducts.visible(isWarehouseSelected);
}


void WarehouseReport::filterButtonClicked()
{
	btnShowAll.toggle(false);
	btnVacancy.toggle(false);
	btnFull.toggle(false);
	btnEmpty.toggle(false);
	btnDisabled.toggle(false);
}


void WarehouseReport::onShowAll()
{
	filterButtonClicked();
	btnShowAll.toggle(true);

	fillLists();
}


void WarehouseReport::onFull()
{
	filterButtonClicked();
	btnFull.toggle(true);

	fillListFull();
}


void WarehouseReport::onVacancy()
{
	filterButtonClicked();
	btnVacancy.toggle(true);

	fillListVacancy();
}


void WarehouseReport::onEmpty()
{
	filterButtonClicked();
	btnEmpty.toggle(true);

	fillListEmpty();
}


void WarehouseReport::onDisabled()
{
	filterButtonClicked();
	btnDisabled.toggle(true);

	fillListDisabled();
}


void WarehouseReport::onTakeMeThere()
{
	const auto* warehouse = selectedWarehouse();
	if (warehouse)
	{
		if (mTakeMeThereHandler) { mTakeMeThereHandler(warehouse); }
	}
}


void WarehouseReport::onStructureSelectionChange()
{
	const auto* warehouse = selectedWarehouse();
	if (warehouse != nullptr)
	{
		lstProducts.productPool(warehouse->products());
	}
	setVisibility();
}

void WarehouseReport::drawLeftPanel(Renderer& renderer) const
{
	const auto textLineSpacing = infoSectionHeight / 3;
	const auto textOrigin = position() + infoSectionOffset;
	renderer.drawText(fontMediumBold, "Warehouse Count", textOrigin, constants::PrimaryTextColor);
	renderer.drawText(fontMediumBold, "Total Storage", textOrigin + NAS2D::Vector{0, textLineSpacing}, constants::PrimaryTextColor);
	renderer.drawText(fontMediumBold, "Capacity Used", textOrigin + NAS2D::Vector{0, textLineSpacing * 2}, constants::PrimaryTextColor);

	const auto valueOrigin = textOrigin + NAS2D::Vector{mRect.size.x / 2 - 20, fontMediumBold.height() - fontMedium.height()};
	const auto warehouseCountText = std::to_string(warehouseCount);
	const auto warehouseCapacityText = std::to_string(warehouseCapacityTotal);
	const auto countTextWidth = fontMedium.width(warehouseCountText);
	const auto capacityTextWidth = fontMedium.width(warehouseCapacityText);
	renderer.drawText(fontMedium, warehouseCountText, valueOrigin + NAS2D::Vector{-countTextWidth, 0}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, warehouseCapacityText, valueOrigin + NAS2D::Vector{-capacityTextWidth, textLineSpacing}, constants::PrimaryTextColor);

	const auto capacityUsedTextWidth = fontMediumBold.width("Capacity Used");
	const auto capacityBarPosition = textOrigin + NAS2D::Vector{capacityUsedTextWidth + 10, 3 + textLineSpacing * 2};
	const auto capacityBarSize = NAS2D::Vector{valueOrigin.x - capacityBarPosition.x, 20};
	drawProgressBar(
		warehouseCapacityUsed,
		warehouseCapacityTotal,
		{capacityBarPosition, capacityBarSize}
	);
}


void WarehouseReport::drawRightPanel(Renderer& renderer) const
{
	const auto* warehouse = selectedWarehouse();
	if (!warehouse) { return; }

	const auto position = NAS2D::Point{renderer.center().x + 10, this->position().y};
	renderer.drawText(fontBigBold, warehouse->name(), position + NAS2D::Vector{0, 2}, constants::PrimaryTextColor);
	renderer.drawImage(imageWarehouse, position + NAS2D::Vector{0, 35});
}


void WarehouseReport::update()
{
	ControlContainer::update();
	auto& renderer = Utility<Renderer>::get();
	draw(renderer);
}


void WarehouseReport::draw(NAS2D::Renderer& renderer) const
{
	// Left Panel
	drawLeftPanel(renderer);
	const auto position = NAS2D::Point{renderer.center().x, this->position().y};
	renderer.drawLine(position + NAS2D::Vector{0, 10}, position + NAS2D::Vector{0, mRect.size.y - 10}, constants::PrimaryColor);
	drawRightPanel(renderer);
}
