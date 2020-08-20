// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProductListBox.h"

#include "../Constants.h"
#include "../Cache.h"
#include "../ProductPool.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


namespace {
	constexpr int LIST_ITEM_HEIGHT = 30;

	const Font* MAIN_FONT = nullptr;
	const Font* MAIN_FONT_BOLD = nullptr;
}


/**
 * C'tor
 */
ProductListBox::ProductListBox()
{
	item_height(LIST_ITEM_HEIGHT);
	MAIN_FONT = &fontCache.load(constants::FONT_PRIMARY, 12);
	MAIN_FONT_BOLD = &fontCache.load(constants::FONT_PRIMARY_BOLD, 12);
}


/**
 * Fills the ProductListBox with Products in a ProductPool object.
 */
void ProductListBox::productPool(ProductPool& pool)
{
	clearItems();

	for (std::size_t product_type = 0; product_type < static_cast<std::size_t>(ProductType::PRODUCT_COUNT); ++product_type)
	{
		if (pool.count(static_cast<ProductType>(product_type)) > 0)
		{
			ProductListBoxItem* item = new ProductListBoxItem();
			item->Text = productDescription(static_cast<ProductType>(product_type));
			item->count = pool.count(static_cast<ProductType>(product_type));
			item->usage = static_cast<float>(item->count) / static_cast<float>(pool.capacity());
			mItems.push_back(item);
		}
	}

	_update_item_display();
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

	constexpr Color ITEM_COLOR{0, 185, 0};
	constexpr Color HIGHLIGHT_COLOR{0, 185, 0, 75};

	const auto FIRST_STOP = static_cast<int>(item_width() * 0.33f);
	const auto SECOND_STOP = static_cast<int>(item_width() * 0.66f);

	for (std::size_t i = 0; i < mItems.size(); ++i)
	{
		const auto& item = *static_cast<ProductListBoxItem*>(mItems[i]);
		const auto x = positionX();
		const auto y = positionY() + (static_cast<int>(i) * LIST_ITEM_HEIGHT);
		const auto w = static_cast<int>(item_width());
		const auto offset = static_cast<int>(draw_offset());
		const auto highlight = i == currentSelection();

		// draw highlight rect so as not to tint/hue colors of everything else
		if (highlight) { renderer.drawBoxFilled(NAS2D::Rectangle{x, y - offset, w, LIST_ITEM_HEIGHT}, HIGHLIGHT_COLOR); }

		renderer.drawBox(NAS2D::Rectangle{x + 2, y + 2 - offset, w - 4, LIST_ITEM_HEIGHT - 4}, ITEM_COLOR);

		renderer.drawLine(NAS2D::Point{x + FIRST_STOP, y + 2}, NAS2D::Point{x + FIRST_STOP, y + LIST_ITEM_HEIGHT - 2}, ITEM_COLOR);
		renderer.drawLine(NAS2D::Point{x + SECOND_STOP, y + 2}, NAS2D::Point{x + SECOND_STOP, y + LIST_ITEM_HEIGHT - 2}, ITEM_COLOR);

		renderer.drawText(*MAIN_FONT_BOLD, item.Text, NAS2D::Point{x + 5, ((y + 15) - MAIN_FONT_BOLD->height() / 2) - offset}, ITEM_COLOR);

		renderer.drawText(*MAIN_FONT, "Quantity: " + std::to_string(item.count), NAS2D::Point{x + FIRST_STOP + 5, ((y + 15) - MAIN_FONT_BOLD->height() / 2)}, ITEM_COLOR);

		drawBasicProgressBar(x + SECOND_STOP + 5, y + 10, FIRST_STOP - 10, 10, item.usage, 2);
	}

	renderer.clipRectClear();
}
