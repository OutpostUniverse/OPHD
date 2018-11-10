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

}


/**
 * Sets up UI positions.
 */
void WarehouseReport::init()
{

}


void WarehouseReport::fillLists()
{

}


void WarehouseReport::clearSelection()
{

}


/**
 * 
 */
void WarehouseReport::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBox(100, 100, 100, 100, 255, 0, 255);

	UIContainer::update();
}

