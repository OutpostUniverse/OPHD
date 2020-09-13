#include "WarehouseInspector.h"

#include "TextRender.h"
#include "../Constants.h"

#include "../Things/Structures/Warehouse.h"

using namespace NAS2D;

/**
 * 
 */
WarehouseInspector::WarehouseInspector() :
	btnClose{"Okay"}
{
	text(constants::WINDOW_WH_INSPECTOR);
	init();
}


WarehouseInspector::~WarehouseInspector()
{}


/**
 * 
 */
void WarehouseInspector::init()
{
	size({250, 350});

	add(&btnClose, 105, 325);
	btnClose.size({40, 20});
	btnClose.click().connect(this, &WarehouseInspector::btnCloseClicked);
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

	ProductPool& pool = mWarehouse->products();

	const int labelWidth = 100;

	auto position = mRect.startPoint() + NAS2D::Vector{constants::MARGIN, 25};
	drawLabelAndValueLeftJustify(position, labelWidth, "Storage:", std::to_string(pool.availableStorage()) + " / " + std::to_string(pool.capacity()));

	position.y += 25;
	drawLabelAndValueLeftJustify(position, labelWidth, "Clothing:", std::to_string(pool.count(ProductType::PRODUCT_CLOTHING)));

	position.y += 15;
	drawLabelAndValueLeftJustify(position, labelWidth, "Medicine:", std::to_string(pool.count(ProductType::PRODUCT_MEDICINE)));

	position.y += 15;
	drawLabelAndValueLeftJustify(position, labelWidth, "Road Materials:", std::to_string(pool.count(ProductType::PRODUCT_ROAD_MATERIALS)));
}
