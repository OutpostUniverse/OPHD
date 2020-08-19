// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "WarehouseReport.h"

#include "../../Cache.h"
#include "../../Constants.h"
#include "../../StructureManager.h"

#include "../../Things/Structures/Warehouse.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


namespace {
	static const Font* FONT_BOLD = nullptr;
	static const Font* FONT_MED = nullptr;
	static const Font* FONT_MED_BOLD = nullptr;
	static const Font* FONT_BIG_BOLD = nullptr;

	static const Image* WAREHOUSE_IMG = nullptr;

	static int COUNT_WIDTH = 0;
	static int CAPACITY_WIDTH = 0;

	static int CAPACITY_BAR_WIDTH = 0;
	static int CAPACITY_BAR_POSITION_X = 0;

	static float CAPACITY_PERCENT = 0.0f;

	static std::string WH_COUNT;
	static std::string WH_CAPACITY;

	static Warehouse* SELECTED_WAREHOUSE = nullptr;


	/**
	 * Internal function to determine current capacity of all
	 * warehouses in the game.
	 */
	static void computeCapacity()
	{
		int capacity_total = 0;
		int available_capacity = 0;

		const auto& structures = Utility<StructureManager>::get().structureList(Structure::StructureClass::Warehouse);
		for (auto warehouse : structures)
		{
			if (!warehouse->operational()) { continue; } // yuck
			Warehouse* _wh = static_cast<Warehouse*>(warehouse);
			available_capacity += _wh->products().availableStorage();
			capacity_total += _wh->products().capacity();
		}

		int capacity_used = capacity_total - available_capacity;

		WH_COUNT = std::to_string(structures.size());
		WH_CAPACITY = std::to_string(capacity_total);

		COUNT_WIDTH = FONT_MED->width(WH_COUNT);
		CAPACITY_WIDTH = FONT_MED->width(WH_CAPACITY);

		CAPACITY_PERCENT = static_cast<float>(capacity_used) / static_cast<float>(capacity_total);
	}
}


WarehouseReport::WarehouseReport() :
	btnShowAll{"All"},
	btnSpaceAvailable{"Space Available"},
	btnFull{"Full"},
	btnEmpty{"Empty"},
	btnDisabled{"Disabled"},
	btnTakeMeThere{constants::BUTTON_TAKE_ME_THERE}
{
	FONT_BOLD = &fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
	FONT_MED = &fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	FONT_MED_BOLD = &fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM);
	FONT_BIG_BOLD = &fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_HUGE);

	WAREHOUSE_IMG = &imageCache.load("ui/interface/warehouse.png");

	add(&btnShowAll, 10, 10);
	btnShowAll.size({75, 20});
	btnShowAll.type(Button::Type::BUTTON_TOGGLE);
	btnShowAll.toggle(true);
	btnShowAll.click().connect(this, &WarehouseReport::btnShowAllClicked);

	add(&btnSpaceAvailable, 90, 10);
	btnSpaceAvailable.size({100, 20});
	btnSpaceAvailable.type(Button::Type::BUTTON_TOGGLE);
	btnSpaceAvailable.toggle(false);
	btnSpaceAvailable.click().connect(this, &WarehouseReport::btnSpaceAvailableClicked);

	add(&btnFull, 195, 10);
	btnFull.size({75, 20});
	btnFull.type(Button::Type::BUTTON_TOGGLE);
	btnFull.toggle(false);
	btnFull.click().connect(this, &WarehouseReport::btnFullClicked);

	add(&btnEmpty, 275, 10);
	btnEmpty.size({75, 20});
	btnEmpty.type(Button::Type::BUTTON_TOGGLE);
	btnEmpty.toggle(false);
	btnEmpty.click().connect(this, &WarehouseReport::btnEmptyClicked);

	add(&btnDisabled, 355, 10);
	btnDisabled.size({75, 20});
	btnDisabled.type(Button::Type::BUTTON_TOGGLE);
	btnDisabled.toggle(false);
	btnDisabled.click().connect(this, &WarehouseReport::btnDisabledClicked);

	add(&btnTakeMeThere, 10, 10);
	btnTakeMeThere.size({140, 30});
	btnTakeMeThere.click().connect(this, &WarehouseReport::btnTakeMeThereClicked);

	add(&lstStructures, 10, mRect.y + 115);
	lstStructures.selectionChanged().connect(this, &WarehouseReport::lstStructuresSelectionChanged);

	add(&lstProducts, Utility<Renderer>::get().center().x + 10, mRect.y + 173);

	Utility<EventHandler>::get().mouseDoubleClick().connect(this, &WarehouseReport::doubleClicked);

	Control::resized().connect(this, &WarehouseReport::_resized);
	fillLists();
}


WarehouseReport::~WarehouseReport()
{
	Control::resized().disconnect(this, &WarehouseReport::_resized);
}


void WarehouseReport::_fillListFromStructureList(StructureList& list)
{
	for (auto structure : list)
	{
		lstStructures.addItem(structure);
		StructureListBox::StructureListBoxItem* item = lstStructures.last();

		// \fixme	Abuse of interface to achieve custom results.
		ProductPool& products = static_cast<Warehouse*>(structure)->products();

		if (structure->state() != StructureState::Operational) { item->structureState = structure->stateDescription(); }
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
	lstStructures.clearItems();

	_fillListFromStructureList(Utility<StructureManager>::get().structureList(Structure::StructureClass::Warehouse));

	lstStructures.setSelection(0);
	computeCapacity();
}


void WarehouseReport::fillListSpaceAvailable()
{
	lstStructures.clearItems();

	StructureList list;
	for (auto structure : Utility<StructureManager>::get().structureList(Structure::StructureClass::Warehouse))
	{
		Warehouse* wh = static_cast<Warehouse*>(structure);
		if (!wh->products().atCapacity() && !wh->products().empty() && (wh->operational() || wh->isIdle()))
		{
			list.push_back(structure);
		}
	}

	_fillListFromStructureList(list);

	lstStructures.setSelection(0);
	computeCapacity();
}



void WarehouseReport::fillListFull()
{
	lstStructures.clearItems();

	StructureList list;
	for (auto structure : Utility<StructureManager>::get().structureList(Structure::StructureClass::Warehouse))
	{
		Warehouse* wh = static_cast<Warehouse*>(structure);
		if (wh->products().atCapacity() && (wh->operational() || wh->isIdle()))
		{
			list.push_back(structure);
		}
	}

	_fillListFromStructureList(list);

	lstStructures.setSelection(0);
	computeCapacity();
}


void WarehouseReport::fillListEmpty()
{
	lstStructures.clearItems();

	StructureList list;
	for (auto structure : Utility<StructureManager>::get().structureList(Structure::StructureClass::Warehouse))
	{
		Warehouse* wh = static_cast<Warehouse*>(structure);
		if (wh->products().empty() && (wh->operational() || wh->isIdle()))
		{
			list.push_back(structure);
		}
	}

	_fillListFromStructureList(list);

	lstStructures.setSelection(0);
	computeCapacity();
}


void WarehouseReport::fillListDisabled()
{
	lstStructures.clearItems();

	StructureList list;
	for (auto structure : Utility<StructureManager>::get().structureList(Structure::StructureClass::Warehouse))
	{
		if (structure->disabled() || structure->destroyed())
		{
			list.push_back(structure);
		}
	}

	_fillListFromStructureList(list);

	lstStructures.setSelection(0);
	computeCapacity();
}


void WarehouseReport::doubleClicked(EventHandler::MouseButton button, int x, int y)
{
	if (!visible()) { return; }
	if (button != EventHandler::MouseButton::BUTTON_LEFT) { return; }

	if (SELECTED_WAREHOUSE && lstStructures.rect().contains(NAS2D::Point{x, y}))
	{
		takeMeThereCallback()(SELECTED_WAREHOUSE);
	}
}


void WarehouseReport::clearSelection()
{
	lstStructures.clearSelection();
	SELECTED_WAREHOUSE = nullptr;
}


void WarehouseReport::refresh()
{
	btnShowAllClicked();
}


void WarehouseReport::selectStructure(Structure* structure)
{
	lstStructures.currentSelection(structure);
	SELECTED_WAREHOUSE = static_cast<Warehouse*>(structure);
}


void WarehouseReport::_resized(Control*)
{
	lstStructures.size({(mRect.width / 2) - 20, mRect.height - 126});
	lstProducts.size({(mRect.width / 2) - 20, mRect.height - 184});
	lstProducts.position({Utility<Renderer>::get().center().x + 10, lstProducts.positionY()});

	btnTakeMeThere.position({Utility<Renderer>::get().size().x - 150, positionY() + 35});

	CAPACITY_BAR_WIDTH = mRect.width / 2 - 30 - FONT_MED_BOLD->width("Capacity Used");
	CAPACITY_BAR_POSITION_X = 20 + FONT_MED_BOLD->width("Capacity Used");
}


void WarehouseReport::filterButtonClicked()
{
	btnShowAll.toggle(false);
	btnSpaceAvailable.toggle(false);
	btnFull.toggle(false);
	btnEmpty.toggle(false);
	btnDisabled.toggle(false);
}


void WarehouseReport::btnShowAllClicked()
{
	filterButtonClicked();
	btnShowAll.toggle(true);

	fillLists();
}


void WarehouseReport::btnSpaceAvailableClicked()
{
	filterButtonClicked();
	btnSpaceAvailable.toggle(true);

	fillListSpaceAvailable();
}


void WarehouseReport::btnFullClicked()
{
	filterButtonClicked();
	btnFull.toggle(true);

	fillListFull();
}


void WarehouseReport::btnEmptyClicked()
{
	filterButtonClicked();
	btnEmpty.toggle(true);

	fillListEmpty();
}


void WarehouseReport::btnDisabledClicked()
{
	filterButtonClicked();
	btnDisabled.toggle(true);

	fillListDisabled();
}


void WarehouseReport::btnTakeMeThereClicked()
{
	takeMeThereCallback()(SELECTED_WAREHOUSE);
}


void WarehouseReport::lstStructuresSelectionChanged()
{
	SELECTED_WAREHOUSE = static_cast<Warehouse*>(lstStructures.selectedStructure());

	if (SELECTED_WAREHOUSE != nullptr)
	{
		lstProducts.productPool(SELECTED_WAREHOUSE->products());
	}

	btnTakeMeThere.visible(SELECTED_WAREHOUSE != nullptr);
}


void WarehouseReport::drawLeftPanel(Renderer& renderer)
{
	const auto textColor = NAS2D::Color{0, 185, 0};
	renderer.drawText(*FONT_MED_BOLD, "Warehouse Count", NAS2D::Point{10, positionY() + 40}, textColor);
	renderer.drawText(*FONT_MED_BOLD, "Total Storage", NAS2D::Point{10, positionY() + 62}, textColor);
	renderer.drawText(*FONT_MED_BOLD, "Capacity Used", NAS2D::Point{10, positionY() + 84}, textColor);

	renderer.drawText(*FONT_MED, WH_COUNT, NAS2D::Point{mRect.width / 2 - 10 - COUNT_WIDTH, positionY() + 35}, textColor);
	renderer.drawText(*FONT_MED, WH_CAPACITY, NAS2D::Point{mRect.width / 2 - 10 - CAPACITY_WIDTH, positionY() + 57}, textColor);

	drawBasicProgressBar(CAPACITY_BAR_POSITION_X, positionY() + 84, CAPACITY_BAR_WIDTH, 20, CAPACITY_PERCENT);
}


void WarehouseReport::drawRightPanel(Renderer& renderer)
{
	if (!SELECTED_WAREHOUSE) { return; }

	const auto positionX = renderer.center().x + 10;
	renderer.drawText(*FONT_BIG_BOLD, SELECTED_WAREHOUSE->name(), NAS2D::Point{positionX, positionY() + 2}, NAS2D::Color{0, 185, 0});
	renderer.drawImage(*WAREHOUSE_IMG, NAS2D::Point{positionX, positionY() + 35});
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
