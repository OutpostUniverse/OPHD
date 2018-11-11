// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "WarehouseReport.h"


using namespace NAS2D;


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

	Control::resized().connect(this, &WarehouseReport::resized);
	fillLists();
}


void WarehouseReport::fillLists()
{

}


void WarehouseReport::clearSelection()
{

}


void WarehouseReport::refresh()
{

}


void WarehouseReport::selectStructure(Structure* structure)
{

}


void WarehouseReport::resized(Control*)
{

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

	r.drawLine(r.center_x(), rect().y() + 10, r.center_x(), rect().y() + rect().height() - 10, 0, 185, 0);

	UIContainer::update();
}

