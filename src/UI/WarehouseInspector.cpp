// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "WarehouseInspector.h"

#include "../Constants.h"
#include "../FontManager.h"

using namespace NAS2D;

static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;

/**
 * 
 */
WarehouseInspector::WarehouseInspector() :
	btnClose{"Okay"}
{
	text(constants::WINDOW_WH_INSPECTOR);
	init();
}


/**
 * D'tor
 */
WarehouseInspector::~WarehouseInspector()
{}


/**
 * 
 */
void WarehouseInspector::init()
{
	size(250, 350);

	add(&btnClose, 105, 325);
	btnClose.size(40, 20);
	btnClose.click().connect(this, &WarehouseInspector::btnCloseClicked);

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
}


/**
 * 
 */
void WarehouseInspector::warehouse(Warehouse* w)
{
	mWarehouse = w;
}


/**
 *
 */
void WarehouseInspector::btnCloseClicked()
{
	hide();
}


/**
 * 
 */
void WarehouseInspector::hide()
{
	Control::hide();
	mWarehouse = nullptr;
}


/**
 * 
 */
void WarehouseInspector::update()
{
	if (!visible()) { return; }

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	ProductPool& _pool = mWarehouse->products();

	r.drawText(*FONT_BOLD, "Storage", rect().x() + constants::MARGIN, rect().y() + 25.0f, 255, 255, 255);
	r.drawText(*FONT, string_format("%i / %i", _pool.availableStorage(), _pool.capacity()), rect().x() + constants::MARGIN + FONT_BOLD->width("Storage") + 20, rect().y() + 25.0f, 255, 255, 255);

	r.drawText(*FONT_BOLD, "Clothing:", rect().x() + constants::MARGIN, rect().y() + 50.0f, 255, 255, 255);
	r.drawText(*FONT, std::to_string(_pool.count(PRODUCT_CLOTHING)), rect().x() + constants::MARGIN + 100, rect().y() + 50.0f, 255, 255, 255);

	r.drawText(*FONT_BOLD, "Medicine:", rect().x() + constants::MARGIN, rect().y() + 65.0f, 255, 255, 255);
	r.drawText(*FONT, std::to_string(_pool.count(PRODUCT_MEDICINE)), rect().x() + constants::MARGIN + 100, rect().y() + 65.0f, 255, 255, 255);

	r.drawText(*FONT_BOLD, "Road Materials:", rect().x() + constants::MARGIN, rect().y() + 80.0f, 255, 255, 255);
	r.drawText(*FONT, std::to_string(_pool.count(PRODUCT_ROAD_MATERIALS)), rect().x() + constants::MARGIN + 100, rect().y() + 80.0f, 255, 255, 255);
}
