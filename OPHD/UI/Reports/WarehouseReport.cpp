#include "WarehouseReport.h"

#include "../../Cache.h"
#include "../../Constants.h"
#include "../../StructureManager.h"
#include "../../Things/Structures/Structure.h"
#include "../../Things/Structures/Warehouse.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>


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
}


WarehouseReport::WarehouseReport() :
	fontMedium{fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM)},
	fontMediumBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM)},
	fontBigBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_HUGE)},
	imageWarehouse{imageCache.load("ui/interface/warehouse.png")},
	btnShowAll{"All"},
	btnSpaceAvailable{"Space Available"},
	btnFull{"Full"},
	btnEmpty{"Empty"},
	btnDisabled{"Disabled"},
	btnTakeMeThere{constants::BUTTON_TAKE_ME_THERE}
{
	add(btnShowAll, {10, 10});
	btnShowAll.size({75, 20});
	btnShowAll.type(Button::Type::BUTTON_TOGGLE);
	btnShowAll.toggle(true);
	btnShowAll.click().connect(this, &WarehouseReport::onShowAll);

	add(btnSpaceAvailable, {90, 10});
	btnSpaceAvailable.size({100, 20});
	btnSpaceAvailable.type(Button::Type::BUTTON_TOGGLE);
	btnSpaceAvailable.toggle(false);
	btnSpaceAvailable.click().connect(this, &WarehouseReport::onSpaceAvailable);

	add(btnFull, {195, 10});
	btnFull.size({75, 20});
	btnFull.type(Button::Type::BUTTON_TOGGLE);
	btnFull.toggle(false);
	btnFull.click().connect(this, &WarehouseReport::onFull);

	add(btnEmpty, {275, 10});
	btnEmpty.size({75, 20});
	btnEmpty.type(Button::Type::BUTTON_TOGGLE);
	btnEmpty.toggle(false);
	btnEmpty.click().connect(this, &WarehouseReport::onEmpty);

	add(btnDisabled, {355, 10});
	btnDisabled.size({75, 20});
	btnDisabled.type(Button::Type::BUTTON_TOGGLE);
	btnDisabled.toggle(false);
	btnDisabled.click().connect(this, &WarehouseReport::onDisabled);

	add(btnTakeMeThere, {10, 10});
	btnTakeMeThere.size({140, 30});
	btnTakeMeThere.click().connect(this, &WarehouseReport::onTakeMeThere);

	add(lstStructures, {10, mRect.y + 115});
	lstStructures.selectionChanged().connect(this, &WarehouseReport::onStructureSelectionChange);

	add(lstProducts, {Utility<Renderer>::get().center().x + 10, mRect.y + 173});

	Utility<EventHandler>::get().mouseDoubleClick().connect(this, &WarehouseReport::onDoubleClick);

	fillLists();
}


WarehouseReport::~WarehouseReport()
{
	Utility<EventHandler>::get().mouseDoubleClick().disconnect(this, &WarehouseReport::onDoubleClick);
}


void WarehouseReport::computeTotalWarehouseCapacity()
{
	int capacityTotal = 0;
	int capacityAvailable = 0;

	const auto& warehouses = Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto warehouse : warehouses)
	{
		if (warehouse->operational())
		{
			const auto& warehouseProducts = warehouse->products();
			capacityAvailable += warehouseProducts.availableStorage();
			capacityTotal += warehouseProducts.capacity();
		}
	}

	int capacityUsed = capacityTotal - capacityAvailable;

	warehouseCount = warehouses.size();
	warehouseCapacityTotal = capacityTotal;
	warehouseCapacityPercent = static_cast<float>(capacityUsed) / static_cast<float>(capacityTotal);
}


void WarehouseReport::_fillListFromStructureList(const std::vector<Warehouse*>& warehouses)
{
	for (auto warehouse : warehouses)
	{
		lstStructures.addItem(warehouse);
		StructureListBox::StructureListBoxItem* item = lstStructures.last();

		// \fixme	Abuse of interface to achieve custom results.
		ProductPool& products = warehouse->products();

		if (warehouse->state() != StructureState::Operational) { item->structureState = warehouse->stateDescription(); }
		else if (products.empty()) { item->structureState = constants::WAREHOUSE_EMPTY; }
		else if (products.atCapacity()) { item->structureState = constants::WAREHOUSE_FULL; }
		else if (!products.empty() && !products.atCapacity()) { item->structureState = constants::WAREHOUSE_SPACE_AVAILABLE; }
	}
}


/**
 * Inherited interface. A better name for this function would be
 * fillListWithAll() or something to that effect.
 */
void WarehouseReport::fillLists()
{
	lstStructures.clear();

	_fillListFromStructureList(selectWarehouses([](Warehouse*) { return true; }));

	lstStructures.setSelection(0);
	computeTotalWarehouseCapacity();
}


void WarehouseReport::fillListSpaceAvailable()
{
	lstStructures.clear();

	const auto predicate = [](Warehouse* wh) {
		return !wh->products().atCapacity() && !wh->products().empty() && (wh->operational() || wh->isIdle());
	};

	_fillListFromStructureList(selectWarehouses(predicate));

	lstStructures.setSelection(0);
	computeTotalWarehouseCapacity();
}



void WarehouseReport::fillListFull()
{
	lstStructures.clear();

	const auto predicate = [](Warehouse* wh) {
		return wh->products().atCapacity() && (wh->operational() || wh->isIdle());
	};

	_fillListFromStructureList(selectWarehouses(predicate));

	lstStructures.setSelection(0);
	computeTotalWarehouseCapacity();
}


void WarehouseReport::fillListEmpty()
{
	lstStructures.clear();

	const auto predicate = [](Warehouse* wh) {
		return wh->products().empty() && (wh->operational() || wh->isIdle());
	};

	_fillListFromStructureList(selectWarehouses(predicate));

	lstStructures.setSelection(0);
	computeTotalWarehouseCapacity();
}


void WarehouseReport::fillListDisabled()
{
	lstStructures.clear();

	const auto predicate = [](Warehouse* structure) {
		return structure->disabled() || structure->destroyed();
	};

	_fillListFromStructureList(selectWarehouses(predicate));

	lstStructures.setSelection(0);
	computeTotalWarehouseCapacity();
}


void WarehouseReport::onDoubleClick(EventHandler::MouseButton button, int x, int y)
{
	if (!visible()) { return; }
	if (button != EventHandler::MouseButton::Left) { return; }

	if (selectedWarehouse && lstStructures.rect().contains(NAS2D::Point{x, y}))
	{
		takeMeThereSignal()(selectedWarehouse);
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

	lstStructures.size({(mRect.width / 2) - 20, mRect.height - 126});
	lstProducts.size({(mRect.width / 2) - 20, mRect.height - 184});
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
	takeMeThereSignal()(selectedWarehouse);
}


void WarehouseReport::onStructureSelectionChange()
{
	selectedWarehouse = static_cast<Warehouse*>(lstStructures.selectedStructure());

	if (selectedWarehouse != nullptr)
	{
		lstProducts.productPool(selectedWarehouse->products());
	}

	btnTakeMeThere.visible(selectedWarehouse != nullptr);
}


void WarehouseReport::drawLeftPanel(Renderer& renderer)
{
	const auto textColor = NAS2D::Color{0, 185, 0};
	renderer.drawText(fontMediumBold, "Warehouse Count", NAS2D::Point{10, positionY() + 40}, textColor);
	renderer.drawText(fontMediumBold, "Total Storage", NAS2D::Point{10, positionY() + 62}, textColor);
	renderer.drawText(fontMediumBold, "Capacity Used", NAS2D::Point{10, positionY() + 84}, textColor);

	const auto warehouseCountText = std::to_string(warehouseCount);
	const auto warehouseCapacityText = std::to_string(warehouseCapacityTotal);
	const auto countTextWidth = fontMedium.width(warehouseCountText);
	const auto capacityTextWidth = fontMedium.width(warehouseCapacityText);
	renderer.drawText(fontMedium, warehouseCountText, NAS2D::Point{mRect.width / 2 - 10 - countTextWidth, positionY() + 35}, textColor);
	renderer.drawText(fontMedium, warehouseCapacityText, NAS2D::Point{mRect.width / 2 - 10 - capacityTextWidth, positionY() + 57}, textColor);

	const auto capacityUsedTextWidth = fontMediumBold.width("Capacity Used");
	const auto capacityBarWidth = mRect.width / 2 - 30 - capacityUsedTextWidth;
	const auto capacityBarPositionX = 20 + capacityUsedTextWidth;
	drawBasicProgressBar(capacityBarPositionX, positionY() + 84, capacityBarWidth, 20, warehouseCapacityPercent);
}


void WarehouseReport::drawRightPanel(Renderer& renderer)
{
	if (!selectedWarehouse) { return; }

	const auto positionX = renderer.center().x + 10;
	renderer.drawText(fontBigBold, selectedWarehouse->name(), NAS2D::Point{positionX, positionY() + 2}, NAS2D::Color{0, 185, 0});
	renderer.drawImage(imageWarehouse, NAS2D::Point{positionX, positionY() + 35});
}


void WarehouseReport::update()
{
	if (!visible()) { return; }
	auto& renderer = Utility<Renderer>::get();

	// Left Panel
	drawLeftPanel(renderer);
	const auto positionX = renderer.center().x;
	renderer.drawLine(NAS2D::Point{positionX, positionY() + 10}, NAS2D::Point{positionX, positionY() + mRect.height - 10}, NAS2D::Color{0, 185, 0});
	drawRightPanel(renderer);

	UIContainer::update();
}
