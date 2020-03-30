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

	Renderer& renderer = Utility<Renderer>::get();

	ProductPool& pool = mWarehouse->products();

	const auto drawTitleText = [&renderer](NAS2D::Point<int> position, const std::string& title, const std::string& text, int offset = 100) {
		renderer.drawText(*FONT_BOLD, title, position, NAS2D::Color::White);
		position.x() += offset ? offset : (FONT_BOLD->width(title) + 20);
		renderer.drawText(*FONT, text, position, NAS2D::Color::White);
	};

	auto position = rect().startPoint() + NAS2D::Vector{constants::MARGIN, 25};
	drawTitleText(position, "Storage", std::to_string(pool.availableStorage()) + " / " + std::to_string(pool.capacity()), 0);

	position.y() += 25;
	drawTitleText(position, "Clothing:", std::to_string(pool.count(PRODUCT_CLOTHING)));

	position.y() += 15;
	drawTitleText(position, "Medicine:", std::to_string(pool.count(PRODUCT_MEDICINE)));

	position.y() += 15;
	drawTitleText(position, "Road Materials:", std::to_string(pool.count(PRODUCT_ROAD_MATERIALS)));
}
