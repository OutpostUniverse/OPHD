#include "WarehouseReport.h"

#include "../ProgressBar.h"

#include "../../Cache.h"
#include "../../StructureManager.h"
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
		else if (!productPool.empty() && !productPool.atCapacity()) { return constants::WarehouseSpaceAvailable; }

		return std::string{};
	}
}


WarehouseReport::WarehouseReport() :
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	imageWarehouse{imageCache.load("ui/interface/warehouse.png")},
	btnShowAll{"All", {this, &WarehouseReport::onShowAll}},
	btnSpaceAvailable{"Space Available", {this, &WarehouseReport::onSpaceAvailable}},
	btnFull{"Full", {this, &WarehouseReport::onFull}},
	btnEmpty{"Empty", {this, &WarehouseReport::onEmpty}},
	btnDisabled{"Disabled", {this, &WarehouseReport::onDisabled}},
	btnTakeMeThere{constants::TakeMeThere, {this, &WarehouseReport::onTakeMeThere}}
{
	const auto buttons = std::array{&btnShowAll, &btnSpaceAvailable, &btnFull, &btnEmpty, &btnDisabled};
	for (auto button : buttons)
	{
		button->size({75, 20});
		button->type(Button::Type::Toggle);
		button->toggle(false);
	}

	btnShowAll.toggle(true);
	btnSpaceAvailable.size({100, 20});

	btnTakeMeThere.size({140, 30});

	lstStructures.selectionChanged().connect({this, &WarehouseReport::onStructureSelectionChange});

	Utility<EventHandler>::get().mouseDoubleClick().connect({this, &WarehouseReport::onDoubleClick});

	fillLists();

	auto buttonOffset = NAS2D::Vector{10, 10};
	const auto margin = 5;
	for (auto button : buttons)
	{
		add(*button, buttonOffset);
		buttonOffset.x += button->size().x + margin;
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


void WarehouseReport::fillListSpaceAvailable()
{
	const auto predicate = [](Warehouse* wh) {
		return !wh->products().atCapacity() && !wh->products().empty() && (wh->operational() || wh->isIdle());
	};

	fillListFromStructureList(selectWarehouses(predicate));
}



void WarehouseReport::fillListFull()
{
	const auto predicate = [](Warehouse* wh) {
		return wh->products().atCapacity() && (wh->operational() || wh->isIdle());
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

	if (selectedWarehouse && lstStructures.rect().contains(position))
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
	lstProducts.position({Utility<Renderer>::get().center().x + 10, lstProducts.positionY()});

	btnTakeMeThere.position({Utility<Renderer>::get().size().x - 150, positionY() + 35});
}


void WarehouseReport::filterButtonClicked()
{
	btnShowAll.toggle(false);
	btnSpaceAvailable.toggle(false);
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


void WarehouseReport::onSpaceAvailable()
{
	filterButtonClicked();
	btnSpaceAvailable.toggle(true);

	fillListSpaceAvailable();
}


void WarehouseReport::onFull()
{
	filterButtonClicked();
	btnFull.toggle(true);

	fillListFull();
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
	renderer.drawText(fontMediumBold, "Warehouse Count", NAS2D::Point{10, positionY() + 40}, constants::PrimaryTextColor);
	renderer.drawText(fontMediumBold, "Total Storage", NAS2D::Point{10, positionY() + 62}, constants::PrimaryTextColor);
	renderer.drawText(fontMediumBold, "Capacity Used", NAS2D::Point{10, positionY() + 84}, constants::PrimaryTextColor);

	const auto warehouseCountText = std::to_string(warehouseCount);
	const auto warehouseCapacityText = std::to_string(warehouseCapacityTotal);
	const auto countTextWidth = fontMedium.width(warehouseCountText);
	const auto capacityTextWidth = fontMedium.width(warehouseCapacityText);
	renderer.drawText(fontMedium, warehouseCountText, NAS2D::Point{mRect.size.x / 2 - 10 - countTextWidth, positionY() + 35}, constants::PrimaryTextColor);
	renderer.drawText(fontMedium, warehouseCapacityText, NAS2D::Point{mRect.size.x / 2 - 10 - capacityTextWidth, positionY() + 57}, constants::PrimaryTextColor);

	const auto capacityUsedTextWidth = fontMediumBold.width("Capacity Used");
	const auto capacityBarWidth = mRect.size.x / 2 - 30 - capacityUsedTextWidth;
	const auto capacityBarPositionX = 20 + capacityUsedTextWidth;
	drawProgressBar(
		warehouseCapacityUsed,
		warehouseCapacityTotal,
		{{capacityBarPositionX, positionY() + 84}, {capacityBarWidth, 20}}
	);
}


void WarehouseReport::drawRightPanel(Renderer& renderer)
{
	if (!selectedWarehouse) { return; }

	const auto positionX = renderer.center().x + 10;
	renderer.drawText(fontBigBold, selectedWarehouse->name(), NAS2D::Point{positionX, positionY() + 2}, constants::PrimaryTextColor);
	renderer.drawImage(imageWarehouse, NAS2D::Point{positionX, positionY() + 35});
}


void WarehouseReport::update()
{
	if (!visible()) { return; }
	auto& renderer = Utility<Renderer>::get();

	// Left Panel
	drawLeftPanel(renderer);
	const auto positionX = renderer.center().x;
	renderer.drawLine(NAS2D::Point{positionX, positionY() + 10}, NAS2D::Point{positionX, positionY() + mRect.size.y - 10}, constants::PrimaryColor);
	drawRightPanel(renderer);

	UIContainer::update();
}
