// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "WarehouseInspector.h"

#include "../Constants.h"

/**
 * 
 */
WarehouseInspector::WarehouseInspector(Font& font) : mBold("fonts/opensans-bold.ttf", 10)
{
	Control::font(font);
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

	addControl("btnClose", &btnClose, 105, 325);
	btnClose.font(font());
	btnClose.text("Okay");
	btnClose.size(40, 20);
	btnClose.click().connect(this, &WarehouseInspector::btnCloseClicked);
}


/**
 * 
 */
void WarehouseInspector::warehouse(Warehouse* _wh)
{
	mWarehouse = _wh;
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

	r.drawText(mBold, "Storage", rect().x() + constants::MARGIN, rect().y() + 25.0f, 255, 255, 255);
	r.drawText(font(), string_format("%i / 100", mWarehouse->products().capacity()), rect().x() + constants::MARGIN + font().width("Storage") + 20, rect().y() + 25.0f, 255, 255, 255);

	r.drawText(mBold, "Clothing:", rect().x() + constants::MARGIN, rect().y() + 50.0f, 255, 255, 255);
	r.drawText(font(), to_string(mWarehouse->products().count(PRODUCT_CLOTHING)), rect().x() + constants::MARGIN + 100, rect().y() + 50.0f, 255, 255, 255);

	r.drawText(mBold, "Medicine:", rect().x() + constants::MARGIN, rect().y() + 65.0f, 255, 255, 255);
	r.drawText(font(), to_string(mWarehouse->products().count(PRODUCT_MEDICINE)), rect().x() + constants::MARGIN + 100, rect().y() + 65.0f, 255, 255, 255);

	r.drawText(mBold, "Road Materials:", rect().x() + constants::MARGIN, rect().y() + 80.0f, 255, 255, 255);
	r.drawText(font(), to_string(mWarehouse->products().count(PRODUCT_ROAD_MATERIALS)), rect().x() + constants::MARGIN + 100, rect().y() + 80.0f, 255, 255, 255);
}
