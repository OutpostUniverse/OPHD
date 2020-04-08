// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProductListBox.h"

#include "../Constants.h"
#include "../FontManager.h"
#include "../ProductPool.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"


using namespace NAS2D;


const int LIST_ITEM_HEIGHT = 30;

static Font* MAIN_FONT = nullptr;
static Font* MAIN_FONT_BOLD = nullptr;


static Color ITEM_COLOR(0, 185, 0, 255);


static int FIRST_STOP = 0;
static int SECOND_STOP = 0;


static void drawItem(Renderer& r, ProductListBox::ProductListBoxItem& item, float x, float y, float w, float offset, bool highlight)
{
	// draw highlight rect so as not to tint/hue colors of everything else
	if (highlight) { r.drawBoxFilled(x, y - offset, w, LIST_ITEM_HEIGHT, ITEM_COLOR.red(), ITEM_COLOR.green(), ITEM_COLOR.blue(), 75); }

	r.drawBox(x + 2, y + 2 - offset, w - 4, LIST_ITEM_HEIGHT - 4, ITEM_COLOR.red(), ITEM_COLOR.green(), ITEM_COLOR.blue(), ITEM_COLOR.alpha());

	r.drawLine(x + FIRST_STOP, y + 2, x + FIRST_STOP, y + LIST_ITEM_HEIGHT - 2, ITEM_COLOR);
	r.drawLine(x + SECOND_STOP, y + 2, x + SECOND_STOP, y + LIST_ITEM_HEIGHT - 2, ITEM_COLOR);

	r.drawText(*MAIN_FONT_BOLD, item.Text, {x + 5, ((y + 15) - MAIN_FONT_BOLD->height() / 2) - offset}, ITEM_COLOR);

	r.drawText(*MAIN_FONT, "Quantity: " + std::to_string(item.count), {x + FIRST_STOP + 5, ((y + 15) - MAIN_FONT_BOLD->height() / 2)}, ITEM_COLOR);
	
	drawBasicProgressBar(x + static_cast<float>(SECOND_STOP) + 5, y + 10, static_cast<float>(FIRST_STOP) - 10, 10, item.usage, 2);
}


/**
 * C'tor
 */
ProductListBox::ProductListBox()
{
	_init();
}


/**
 * 
 */
void ProductListBox::_init()
{
	item_height(LIST_ITEM_HEIGHT);
	MAIN_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 12);
	MAIN_FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 12);
}


/**
 * Fills the ProductListBox with Products in a ProductPool object.
 */
void ProductListBox::productPool(ProductPool& pool)
{
	clearItems();

	for (size_t product_type = 0; product_type < static_cast<size_t>(PRODUCT_COUNT); ++product_type)
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

	Renderer& r = Utility<Renderer>::get();

	r.clipRect(rect());

	FIRST_STOP = static_cast<int>(item_width() * 0.33f);
	SECOND_STOP = static_cast<int>(item_width() * 0.66f);

	for (size_t i = 0; i < mItems.size(); ++i)
	{
		drawItem(r, *static_cast<ProductListBoxItem*>(mItems[i]),
			positionX(),
			positionY() + (i * LIST_ITEM_HEIGHT),
			static_cast<float>(item_width()),
			static_cast<float>(draw_offset()),
			i == ListBoxBase::currentSelection());
	}

	r.clipRectClear();
}
