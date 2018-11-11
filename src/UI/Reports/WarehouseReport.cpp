// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "WarehouseReport.h"

#include "../../Constants.h"
#include "../../FontManager.h"
#include "../../StructureManager.h"

#include "../../Things/Structures/Warehouse.h"


using namespace NAS2D;


static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;
static Font* FONT_MED = nullptr;
static Font* FONT_MED_BOLD = nullptr;
static Font* FONT_BIG = nullptr;
static Font* FONT_BIG_BOLD = nullptr;


static int COUNT_WIDTH = 0;
static int CAPACITY_WIDTH = 0;

static int CAPACITY_BAR_WIDTH = 0;
static int CAPACITY_BAR_POSITION_X = 0;

static float CAPACITY_PERCENT = 0.0f;

static std::string WH_COUNT;
static std::string WH_CAPACITY;


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
}


/**
 * Sets up UI positions.
 */
void WarehouseReport::init()
{
	FONT			= Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	FONT_BOLD		= Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
	FONT_MED		= Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	FONT_MED_BOLD	= Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM);
	FONT_BIG		= Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_HUGE);
	FONT_BIG_BOLD	= Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_HUGE);

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

	add(&lstStructures, 10, rect().y() + 105);

	Control::resized().connect(this, &WarehouseReport::resized);
	fillLists();
}


void WarehouseReport::fillLists()
{
	lstStructures.clearItems();
	for (auto structure : Utility<StructureManager>::get().structureList(Structure::CLASS_WAREHOUSE))
	{
		lstStructures.addItem(structure);
	}
}


void WarehouseReport::clearSelection()
{
	lstStructures.clearSelection();
}


void WarehouseReport::refresh()
{
	btnShowAllClicked();

	StructureList& sl = Utility<StructureManager>::get().structureList(Structure::CLASS_WAREHOUSE);

	COUNT_WIDTH = FONT_MED->width(WH_COUNT);
	CAPACITY_WIDTH = FONT_MED->width(WH_CAPACITY);
	
	int capacity_total = 0;
	int available_capacity = 0;
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

}


void WarehouseReport::resized(Control*)
{
	lstStructures.size((width() / 2) - 20, height() - 116);

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


/**
 * 
 */
void WarehouseReport::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawText(*FONT_MED_BOLD, "Warehouse Count", 10, positionY() + 35, 0, 185, 0);
	r.drawText(*FONT_MED_BOLD, "Total Storage", 10, positionY() + 57, 0, 185, 0);
	r.drawText(*FONT_MED_BOLD, "Capacity Used", 10, positionY() + 79, 0, 185, 0);


	r.drawText(*FONT_MED, WH_COUNT, width() / 2 - 10 - COUNT_WIDTH, positionY() + 35, 0, 185, 0);
	r.drawText(*FONT_MED, WH_CAPACITY, width() / 2 - 10 - CAPACITY_WIDTH, positionY() + 57, 0, 185, 0);

	drawBasicProgressBar(CAPACITY_BAR_POSITION_X, positionY() + 79, CAPACITY_BAR_WIDTH, 20, CAPACITY_PERCENT);


	r.drawLine(r.center_x(), positionY() + 10, r.center_x(), positionY() + height() - 10, 0, 185, 0);

	UIContainer::update();
}
