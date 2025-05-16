#include "WarehouseReport.h"

#include "../ProgressBar.h"

#include "../../Cache.h"
#include "../../StructureManager.h"
#include "../../Constants/Strings.h"
#include "../../Constants/UiConstants.h"
#include "../../MapObjects/Structure.h"
#include "../../MapObjects/Structures/Warehouse.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>
#include <string>
#include <iterator>
#include <algorithm>


using namespace NAS2D;


namespace
{
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

		if (warehouse.state() != StructureState::Operational) { return warehouse.stateDescription(); }
		else if (productPool.empty()) { return constants::WarehouseEmpty; }
		else if (productPool.atCapacity()) { return constants::WarehouseFull; }
		else if (!productPool.empty() && !productPool.atCapacity()) { return constants::WarehouseVacancy; }

		return std::string{};
	}
}


WarehouseReport::WarehouseReport() :
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	imageWarehouse{imageCache.load("ui/interface/warehouse.png")},
	btnShowAll{"All", {this, &WarehouseReport::onShowAll}},
	btnFull{"Full", {this, &WarehouseReport::onFull}},
	btnVacancy{"Vacancy", {this, &WarehouseReport::onVacancy}},
	btnEmpty{"Empty", {this, &WarehouseReport::onEmpty}},
	btnDisabled{"Disabled", {this, &WarehouseReport::onDisabled}},
	btnTakeMeThere{constants::TakeMeThere, {this, &WarehouseReport::onTakeMeThere}}
{
	const auto buttons = std::array{&btnShowAll, &btnVacancy, &btnFull, &btnEmpty, &btnDisabled};
	for (auto button : buttons)
	{
		button->size({89, 20});
		button->type(Button::Type::Toggle);
		button->toggle(false);
	}

	btnShowAll.toggle(true);
	btnVacancy.size({100, 20});

	btnTakeMeThere.size({140, 30});

	lstStructures.selectionChanged().connect({this, &WarehouseReport::onStructureSelectionChange});

	Utility<EventHandler>::get().mouseDoubleClick().connect({this, &WarehouseReport::onDoubleClick});

	fillLists();

	auto buttonOffset = NAS2D::Vector{10, 10};
	for (auto button : buttons)
	{
		add(*button, buttonOffset);
		buttonOffset.x += button->size().x + constants::Margin;
	}
	add(btnTakeMeThere, {10, 10});
	add(lstStructures, {10, 115});
	add(lstProducts, {Utility<Renderer>::get().center().x + 10, 173});
}


WarehouseReport::~WarehouseReport()
{
	Utility<EventHandler>::get().mouseDoubleClick().disconnect({this, &WarehouseReport::onDoubleClick});
}


void WarehouseReport::computeTotalWarehouseCapacity()
{
	int capacityTotal = 0;
	int capacityAvailable = 0;

	const auto& warehouses = Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto* warehouse : warehouses)
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

	lstStructures.setSelection(0);
	computeTotalWarehouseCapacity();
}


/**
 * Inherited interface. A better name for this function would be
 * fillListWithAll() or something to that effect.
 */
void WarehouseReport::fillLists()
{
	fillListFromStructureList(selectWarehouses([](Warehouse*) { return true; }));
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

	if (selectedWarehouse && lstStructures.area().contains(position))
	{
		mTakeMeThereSignal(selectedWarehouse);
	}
}


void WarehouseReport::clearSelected()
{
	lstStructures.clearSelected();
	selectedWarehouse = nullptr;
}


void WarehouseReport::refresh()
{
	onShowAll();
}


void WarehouseReport::selectStructure(Structure* structure)
{
	lstStructures.setSelected(structure);
	selectedWarehouse = static_cast<Warehouse*>(structure);
}


void WarehouseReport::onResize()
{
	Control::onResize();

	lstStructures.size({(mRect.size.x / 2) - 20, mRect.size.y - 126});
	lstProducts.size({(mRect.size.x / 2) - 20, mRect.size.y - 184});
	lstProducts.position({Utility<Renderer>::get().center().x + 10, lstProducts.position().y});

	btnTakeMeThere.position({Utility<Renderer>::get().size().x - 150, position().y + 35});
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
	mTakeMeThereSignal(selectedWarehouse);
}


void WarehouseReport::onStructureSelectionChange()
{
	selectedWarehouse = static_cast<const Warehouse*>(lstStructures.selectedStructure());

	if (selectedWarehouse != nullptr)
	{
		lstProducts.productPool(selectedWarehouse->products());
	}

	btnTakeMeThere.visible(selectedWarehouse != nullptr);
}


void WarehouseReport::drawLeftPanel(Renderer& renderer)
{
	renderer.drawText(fontMediumBold, "Warehouse Count", NAS2D::Point{10, position().y + 40}, constants::PrimaryTextColor);
	renderer.drawText(fontMediumBold, "Total Storage", NAS2D::Point{10, position().y + 62}, constants::PrimaryTextColor);
	renderer.drawText(fontMediumBold, "Capacity Used", NAS2D::Point{10, position().y + 84}, constants::PrimaryTextColor);

	const auto warehouseCountText = std::to_string(warehouseCount);
	const auto warehouseCapacityText = std::to_string(warehouseCapacityTotal);
	const auto countTextWidth = fontMedium.width(warehouseCountText);
	const auto capacityTextWidth = fontMedium.width(warehouseCapacityText);
	renderer.drawText(fontMedium, warehouseCountText, NAS2D::Point{mRect.size.x / 2 - 10 - countTextWidth, position().y + 35}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, warehouseCapacityText, NAS2D::Point{mRect.size.x / 2 - 10 - capacityTextWidth, position().y + 57}, constants::PrimaryTextColor);

	const auto capacityUsedTextWidth = fontMediumBold.width("Capacity Used");
	const auto capacityBarWidth = mRect.size.x / 2 - 30 - capacityUsedTextWidth;
	const auto capacityBarPosition = NAS2D::Point{20 + capacityUsedTextWidth, position().y + 84};
	drawProgressBar(
		warehouseCapacityUsed,
		warehouseCapacityTotal,
		{capacityBarPosition, {capacityBarWidth, 20}}
	);
}


void WarehouseReport::drawRightPanel(Renderer& renderer)
{
	if (!selectedWarehouse) { return; }

	const auto position = NAS2D::Point{renderer.center().x + 10, this->position().y};
	renderer.drawText(fontBigBold, selectedWarehouse->name(), position + NAS2D::Vector{0, 2}, constants::PrimaryTextColor);
	renderer.drawImage(imageWarehouse, position + NAS2D::Vector{0, 35});
}


void WarehouseReport::update()
{
	if (!visible()) { return; }
	auto& renderer = Utility<Renderer>::get();

	// Left Panel
	drawLeftPanel(renderer);
	const auto position = NAS2D::Point{renderer.center().x, this->position().y};
	renderer.drawLine(position + NAS2D::Vector{0, 10}, position + NAS2D::Vector{0, mRect.size.y - 10}, constants::PrimaryColor);
	drawRightPanel(renderer);

	UIContainer::update();
}
