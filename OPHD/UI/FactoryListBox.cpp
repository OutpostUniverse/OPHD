#include "FactoryListBox.h"

#include "../Things/Structures/Factory.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <stdexcept>


using namespace NAS2D;


namespace
{
	const int LIST_ITEM_HEIGHT = 58;
	const Image* STRUCTURE_ICONS = nullptr;

	const Font* MAIN_FONT = nullptr;
	const Font* MAIN_FONT_BOLD = nullptr;
}


static void drawItem(Renderer& renderer, FactoryListBox::FactoryListBoxItem& item, int x, int y, int w, int offset, bool highlight)
{
	Factory* f = item.factory;

	const auto& structureColor = structureColorFromIndex(f->state());
	const auto& structureTextColor = structureTextColorFromIndex(f->state());
	const auto highlightColor = NAS2D::Color{structureColor.red, structureColor.green, structureColor.blue, 75};
	const auto subImageColor = NAS2D::Color{255, 255, 255, structureColor.alpha};

	// draw highlight rect so as not to tint/hue colors of everything else
	if (highlight) { renderer.drawBoxFilled(NAS2D::Rectangle{x, y - offset, w, LIST_ITEM_HEIGHT}, highlightColor); }

	renderer.drawBox(NAS2D::Rectangle{x + 2, y + 2 - offset, w - 4, LIST_ITEM_HEIGHT - 4}, structureColor);
	renderer.drawSubImage(*STRUCTURE_ICONS, NAS2D::Point{x + 8, y + 8 - offset}, NAS2D::Rectangle{item.icon_slice.x, item.icon_slice.y, 46, 46}, subImageColor);

	renderer.drawText(*MAIN_FONT_BOLD, f->name(), NAS2D::Point{x + 64, ((y + 29) - MAIN_FONT_BOLD->height() / 2) - offset}, structureTextColor);

	renderer.drawText(*MAIN_FONT, productDescription(f->productType()), NAS2D::Point{x + w - 112, ((y + 19) - MAIN_FONT_BOLD->height() / 2) - offset}, structureTextColor);

	// PROGRESS BAR
	float percentage = (f->productType() == ProductType::PRODUCT_NONE) ? 0.0f : (f->productionTurnsCompleted() / f->productionTurnsToComplete());
	drawBasicProgressBar(x + w - 112, y + 30 - offset, 105, 11, percentage, 2);
}


FactoryListBox::FactoryListBox()
{
	item_height(LIST_ITEM_HEIGHT);
	STRUCTURE_ICONS = &imageCache.load("ui/structures.png");
	MAIN_FONT = &fontCache.load(constants::FONT_PRIMARY, 12);
	MAIN_FONT_BOLD = &fontCache.load(constants::FONT_PRIMARY_BOLD, 12);
}


/**
 * Adds a Factory to the FactoryListBox.
 * 
 * Specialized version of the default addItem(ListBoxItem*) function.
 */
void FactoryListBox::addItem(Factory* factory)
{
	for (auto item : mItems)
	{
		if (static_cast<FactoryListBoxItem*>(item)->factory == factory)
		{
			throw std::runtime_error("FactoryListBox::addItem(): Can't add factory multiple times");
		}
	}

	const auto& text = factory->name();
	const auto iconPosition = (factory->state() == StructureState::Destroyed) ? NAS2D::Point<int>{414, 368} :
		(text == constants::UndergroundFactory) ? NAS2D::Point<int>{138, 276} :
		(text == constants::SeedFactory) ? NAS2D::Point<int>{460, 368} :
		NAS2D::Point<int>{0, 46}; // Surface factory
	add<FactoryListBoxItem>(text, factory, iconPosition);
}


/**
 * Sets the current selection.
 * 
 * \param f	Pointer to a Factory object. Safe to pass \c nullptr.
 */
void FactoryListBox::setSelected(Factory* f)
{
	if (mItems.empty() || f == nullptr) { return; }
	for (std::size_t i = 0; i < mItems.size(); ++i)
	{
		FactoryListBoxItem* item = static_cast<FactoryListBoxItem*>(mItems[i]);
		if (item->factory == f)
		{
			setSelection(i);
			return;
		}
	}
}


Factory* FactoryListBox::selectedFactory()
{
	return (selectedIndex() == constants::NoSelection) ? nullptr : static_cast<FactoryListBoxItem*>(mItems[selectedIndex()])->factory;
}


/**
 * Draws the FactoryListBox
 */
void FactoryListBox::update()
{
	if (!visible()) { return; }
	ListBoxBase::update();

	auto& renderer = Utility<Renderer>::get();

	renderer.clipRect(mRect);

	// ITEMS
	for (std::size_t i = 0; i < mItems.size(); ++i)
	{
		drawItem(renderer, *static_cast<FactoryListBoxItem*>(mItems[i]),
			positionX(),
			positionY() + (static_cast<int>(i) * LIST_ITEM_HEIGHT),
			static_cast<int>(item_width()),
			static_cast<int>(draw_offset()),
			i == selectedIndex());
	}

	renderer.clipRectClear();
}
