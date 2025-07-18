#include "WarehouseInspector.h"

#include "TextRender.h"

#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include "../MapObjects/Structures/Warehouse.h"

#include <libOPHD/ProductCatalog.h>


using namespace NAS2D;


WarehouseInspector::WarehouseInspector() :
	Window{constants::WindowWarehouseInspector},
	btnClose{"Okay", {this, &WarehouseInspector::onClose}}
{
	size({250, 350});

	add(btnClose, {105, 325});
	btnClose.size({40, 20});
}


void WarehouseInspector::warehouse(const Warehouse* w)
{
	mWarehouse = w;
}


void WarehouseInspector::onClose()
{
	hide();
}


void WarehouseInspector::hide()
{
	Control::hide();
	mWarehouse = nullptr;
}


void WarehouseInspector::drawClientArea(NAS2D::Renderer& /*renderer*/) const
{
	const auto& pool = mWarehouse->products();

	const int labelWidth = 100;

	auto position = mRect.position + NAS2D::Vector{constants::Margin, 25};
	drawLabelAndValueLeftJustify(position, labelWidth, "Storage:", std::to_string(pool.availableStorage()) + " / " + std::to_string(pool.capacity()));

	position.y += 25;

	/**
	 * This could be further improved by caching which products are going to be
	 * displayed and looping through those instead of looping through all 64
	 * products every frame.
	 */
	for (size_t i = 0; i < ProductType::PRODUCT_COUNT; ++i)
	{
		const auto productType = static_cast<ProductType>(i);
		if (pool.count(productType) == 0) { continue; }
		if (storageRequiredPerUnit(productType) == 0) { continue; }

		drawLabelAndValueLeftJustify(position, labelWidth, ProductCatalog::get(productType).Name + ":", std::to_string(pool.count(productType)));

		position.y += 15;
	}
}
