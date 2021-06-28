#include "ProductListBox.h"

#include "../Constants.h"
#include "../Cache.h"
#include "../ProductPool.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


ProductListBox::ProductListBox() :
	mFont{fontCache.load(constants::FONT_PRIMARY, 12)},
	mFontBold{fontCache.load(constants::FONT_PRIMARY_BOLD, 12)}
{
	item_height(30);
}


/**
 * Fills the ProductListBox with Products in a ProductPool object.
 */
void ProductListBox::productPool(ProductPool& pool)
{
	clear();

	const auto productTypeCount = static_cast<std::size_t>(ProductType::PRODUCT_COUNT);

	for (std::size_t product_type = 0; product_type < productTypeCount; ++product_type)
	{
		const auto productType = static_cast<ProductType>(product_type);
		const auto productCount = pool.count(productType);

		if (productCount > 0)
		{
			ProductListBoxItem* item = new ProductListBoxItem();
			item->text = productDescription(productType);
			item->count = productCount;
			item->usage = static_cast<float>(productCount * pool.productStorageRequirement(productType)) / static_cast<float>(pool.capacity());
			mItems.push_back(item);
		}
	}

	updateScrollLayout();
}


/**
 * Draws the FactoryListBox
 */
void ProductListBox::update()
{
	if (!visible()) { return; }
	ListBoxBase::update();

	auto& renderer = Utility<Renderer>::get();

	renderer.clipRect(mRect);

	constexpr Color itemColor{0, 185, 0};
	constexpr Color highlightColor{0, 185, 0, 75};

	const auto itemSize = NAS2D::Vector{item_width(), item_height()}.to<int>();
	const auto firstStop = itemSize.x / 3;
	const auto secondStop = itemSize.x * 2 / 3;
	const auto offset = static_cast<int>(draw_offset());
	const auto x = positionX();

	for (std::size_t i = 0; i < mItems.size(); ++i)
	{
		const auto& item = *static_cast<ProductListBoxItem*>(mItems[i]);
		const auto y = positionY() + (static_cast<int>(i) * itemSize.y);
		const auto highlight = i == selectedIndex();

		// Draw highlight rect so as not to tint/hue colors of everything else
		if (highlight) { renderer.drawBoxFilled(NAS2D::Rectangle{x, y - offset, itemSize.x, itemSize.y}, highlightColor); }

		// Draw item borders and column breaks
		renderer.drawBox(NAS2D::Rectangle{x + 2, y + 2 - offset, itemSize.x - 4, itemSize.y - 4}, itemColor);
		renderer.drawLine(NAS2D::Point{x + firstStop, y + 2}, NAS2D::Point{x + firstStop, y + itemSize.y - 2}, itemColor);
		renderer.drawLine(NAS2D::Point{x + secondStop, y + 2}, NAS2D::Point{x + secondStop, y + itemSize.y - 2}, itemColor);

		// Draw item column contents
		renderer.drawText(mFontBold, item.text, NAS2D::Point{x + 5, ((y + 15) - mFontBold.height() / 2) - offset}, itemColor);
		renderer.drawText(mFont, "Quantity: " + std::to_string(item.count), NAS2D::Point{x + firstStop + 5, ((y + 15) - mFontBold.height() / 2)}, itemColor);
		drawBasicProgressBar(x + secondStop + 5, y + 10, firstStop - 10, 10, item.usage, 2);
	}

	renderer.clipRectClear();
}
