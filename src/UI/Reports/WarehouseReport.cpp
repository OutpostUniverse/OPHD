// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "WarehouseReport.h"

#include "../../Constants.h"
#include "../../FontManager.h"
#include "../../StructureManager.h"

#include "../../Things/Structures/Warehouse.h"


using namespace NAS2D;


static Font* FONT_BOLD = nullptr;
static Font* FONT_MED = nullptr;
static Font* FONT_MED_BOLD = nullptr;
static Font* FONT_BIG_BOLD = nullptr;

static Image* WAREHOUSE_IMG = nullptr;

static int COUNT_WIDTH = 0;
static int CAPACITY_WIDTH = 0;

static int CAPACITY_BAR_WIDTH = 0;
static int CAPACITY_BAR_POSITION_X = 0;

static float CAPACITY_PERCENT = 0.0f;

static std::string WH_COUNT;
static std::string WH_CAPACITY;

static Warehouse* SELECTED_WAREHOUSE = nullptr;


/**
 * Internal convenience function to avoid really fugly code.
 */
static bool useStateString(Structure::StructureState _state)
{
	return _state == Structure::DISABLED || _state == Structure::DESTROYED || _state == Structure::UNDER_CONSTRUCTION || _state == Structure::IDLE;
}


/**
 * C'tor
 */
WarehouseReport::WarehouseReport()
{
	init();
}


/**
 * D'tor
 */
WarehouseReport::~WarehouseReport()
{
	Control::resized().disconnect(this, &WarehouseReport::resized);
	delete WAREHOUSE_IMG;
}


/**
 * Sets up UI positions.
 */
void WarehouseReport::init()
{
	FONT_BOLD		= Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
	FONT_MED		= Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	FONT_MED_BOLD	= Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM);
	FONT_BIG_BOLD	= Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_HUGE);

	WAREHOUSE_IMG = new Image("ui/interface/warehouse.png");

	add(&btnShowAll, 10, 10);
	btnShowAll.size(75, 20);
	btnShowAll.type(Button::BUTTON_TOGGLE);
	btnShowAll.toggle(true);
	btnShowAll.text("All");
	btnShowAll.click().connect(this, &WarehouseReport::btnShowAllClicked);

	add(&btnSpaceAvailable, 90, 10);
	btnSpaceAvailable.size(100, 20);
	btnSpaceAvailable.type(Button::BUTTON_TOGGLE);
	btnSpaceAvailable.toggle(false);
	btnSpaceAvailable.text("Space Available");
	btnSpaceAvailable.click().connect(this, &WarehouseReport::btnSpaceAvailableClicked);

	add(&btnFull, 195, 10);
	btnFull.size(75, 20);
	btnFull.type(Button::BUTTON_TOGGLE);
	btnFull.toggle(false);
	btnFull.text("Full");
	btnFull.click().connect(this, &WarehouseReport::btnFullClicked);

	add(&btnEmpty, 275, 10);
	btnEmpty.size(75, 20);
	btnEmpty.type(Button::BUTTON_TOGGLE);
	btnEmpty.toggle(false);
	btnEmpty.text("Empty");
	btnEmpty.click().connect(this, &WarehouseReport::btnEmptyClicked);

	add(&btnDisabled, 355, 10);
	btnDisabled.size(75, 20);
	btnDisabled.type(Button::BUTTON_TOGGLE);
	btnDisabled.toggle(false);
	btnDisabled.text("Disabled");
	btnDisabled.click().connect(this, &WarehouseReport::btnDisabledClicked);

	add(&lstStructures, 10, rect().y() + 115);
	lstStructures.selectionChanged().connect(this, &WarehouseReport::lstStructuresSelectionChanged);

	Control::resized().connect(this, &WarehouseReport::resized);
	fillLists();
}


void WarehouseReport::fillLists()
{
	lstStructures.clearItems();
	for (auto structure : Utility<StructureManager>::get().structureList(Structure::CLASS_WAREHOUSE))
	{
		lstStructures.addItem(structure);
		StructureListBox::StructureListBoxItem* item = lstStructures.last();
		
		// Hacky -- takes advantage of internal knowledge of how the list boxes
		// work to set up different meta data for warehouse conditions.
		ProductPool& products = static_cast<Warehouse*>(structure)->products();

		if (useStateString(structure->state())) { item->structureState = structureStateDescription(structure->state()); }
		else if(products.empty()) { item->structureState = constants::WAREHOUSE_EMPTY; }
		else if(products.atCapacity()) { item->structureState = constants::WAREHOUSE_FULL; }
		else if(!products.empty() && !products.atCapacity()) { item->structureState = constants::WAREHOUSE_SPACE_AVAILABLE; }
	}

	lstStructures.setSelection(0);
}


void WarehouseReport::clearSelection()
{
	lstStructures.clearSelection();
	SELECTED_WAREHOUSE = nullptr;
}


void WarehouseReport::refresh()
{
	btnShowAllClicked();

	COUNT_WIDTH = FONT_MED->width(WH_COUNT);
	CAPACITY_WIDTH = FONT_MED->width(WH_CAPACITY);
	
	int capacity_total = 0;
	int available_capacity = 0;

	StructureList& sl = Utility<StructureManager>::get().structureList(Structure::CLASS_WAREHOUSE);
	for (auto warehouse : sl)
	{
		Warehouse* _wh = static_cast<Warehouse*>(warehouse);
		available_capacity += _wh->products().availableStorage();
		capacity_total += _wh->products().capacity();
	}

	int capacity_used = capacity_total - available_capacity;

	WH_COUNT = std::to_string(sl.size());
	WH_CAPACITY = std::to_string(capacity_total);

	CAPACITY_PERCENT = static_cast<float>(capacity_used) / static_cast<float>(capacity_total);
}


void WarehouseReport::selectStructure(Structure* structure)
{
	lstStructures.currentSelection(structure);
	SELECTED_WAREHOUSE = static_cast<Warehouse*>(structure);
}


void WarehouseReport::resized(Control*)
{
	lstStructures.size((width() / 2) - 20, height() - 126);

	CAPACITY_BAR_WIDTH = (width() / 2) - 30 - FONT_MED_BOLD->width("Capacity Used");
	CAPACITY_BAR_POSITION_X = 20 + FONT_MED_BOLD->width("Capacity Used");
}


/**
 * 
 */
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
}


void WarehouseReport::btnFullClicked()
{
	filterButtonClicked();
	btnFull.toggle(true);
}


void WarehouseReport::btnEmptyClicked()
{
	filterButtonClicked();
	btnEmpty.toggle(true);
}


void WarehouseReport::btnDisabledClicked()
{
	filterButtonClicked();
	btnDisabled.toggle(true);
}


void WarehouseReport::lstStructuresSelectionChanged()
{
	SELECTED_WAREHOUSE = static_cast<Warehouse*>(lstStructures.selectedStructure());
}


void WarehouseReport::drawLeftPanel(Renderer& r)
{
	r.drawText(*FONT_MED_BOLD, "Warehouse Count", 10, positionY() + 40, 0, 185, 0);
	r.drawText(*FONT_MED_BOLD, "Total Storage", 10, positionY() + 62, 0, 185, 0);
	r.drawText(*FONT_MED_BOLD, "Capacity Used", 10, positionY() + 84, 0, 185, 0);

	r.drawText(*FONT_MED, WH_COUNT, width() / 2 - 10 - COUNT_WIDTH, positionY() + 35, 0, 185, 0);
	r.drawText(*FONT_MED, WH_CAPACITY, width() / 2 - 10 - CAPACITY_WIDTH, positionY() + 57, 0, 185, 0);

	drawBasicProgressBar(CAPACITY_BAR_POSITION_X, positionY() + 79, CAPACITY_BAR_WIDTH, 20, CAPACITY_PERCENT);
}


void WarehouseReport::drawRightPanel(Renderer& r)
{
	if (!SELECTED_WAREHOUSE) { return; }
	
	r.drawText(*FONT_BIG_BOLD, SELECTED_WAREHOUSE->name(), r.center_x() + 10, positionY() + 2, 0, 185, 0);
	r.drawImage(*WAREHOUSE_IMG, r.center_x() + 10, positionY() + 35);

}


/**
 * 
 */
void WarehouseReport::update()
{
	if (!visible()) { return; }
	Renderer& r = Utility<Renderer>::get();

	// Left Panel
	drawLeftPanel(r);
	r.drawLine(r.center_x(), positionY() + 10, r.center_x(), positionY() + height() - 10, 0, 185, 0);
	drawRightPanel(r);

	UIContainer::update();
}
