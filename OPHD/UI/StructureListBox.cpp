#include "StructureListBox.h"

#include "../Things/Structures/Structure.h"

#include "../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <stdexcept>


using namespace NAS2D;

const int LIST_ITEM_HEIGHT = 30;

static const Font* MAIN_FONT = nullptr;
static const Font* MAIN_FONT_BOLD = nullptr;


static void drawItem(Renderer& renderer, StructureListBox::StructureListBoxItem& item, NAS2D::Rectangle<int> rect, bool highlight)
{
	const auto structureState = item.structure->state();
	const auto& structureColor = structureColorFromIndex(structureState);
	const auto& structureTextColor = structureTextColorFromIndex(structureState);
	const auto highlightColor = NAS2D::Color{structureColor.red, structureColor.green, structureColor.blue, 75};

	// draw highlight rect so as not to tint/hue colors of everything else
	if (highlight) { renderer.drawBoxFilled(rect, highlightColor); }

	renderer.drawBox(rect.inset(2), structureColor);

	renderer.drawText(*MAIN_FONT_BOLD, item.text, rect.startPoint() + NAS2D::Vector{5, 15 - MAIN_FONT_BOLD->height() / 2}, structureTextColor);

	renderer.drawText(*MAIN_FONT, item.structureState, rect.crossXPoint() + NAS2D::Vector{-MAIN_FONT->width(item.structureState) - 5, 15 - MAIN_FONT_BOLD->height() / 2}, structureTextColor);
}


StructureListBox::StructureListBoxItem::StructureListBoxItem(Structure* s) :
	ListBoxItem{s->name()},
	structure{s},
	structureState{""},
	colorIndex{s->state()}
{}


StructureListBox::StructureListBox()
{
	item_height(LIST_ITEM_HEIGHT);
	MAIN_FONT = &fontCache.load(constants::FONT_PRIMARY, 12);
	MAIN_FONT_BOLD = &fontCache.load(constants::FONT_PRIMARY_BOLD, 12);
}


/**
 * Adds a Factory to the FactoryListBox.
 * 
 * Specialized version of the default addItem(ListBoxItem*) function.
 */
void StructureListBox::addItem(Structure* structure)
{
	for (auto item : mItems)
	{
		if (static_cast<StructureListBoxItem*>(item)->structure == structure)
		{
			throw std::runtime_error("StructureListBox::addItem(): Can't add structure multiple times");
		}
	}

	add<StructureListBoxItem>(structure);
}


/**
 * Sets the current selection.
 * 
 * \param structure		Pointer to a Structure object. Save to pass \c nullptr.
 */
void StructureListBox::setSelected(Structure* structure)
{
	if (mItems.empty() || structure == nullptr) { return; }

	for (std::size_t i = 0; i < mItems.size(); ++i)
	{
		StructureListBoxItem* item = static_cast<StructureListBoxItem*>(mItems[i]);
		if (item->structure == structure)
		{
			setSelection(i);
			return;
		}
	}
}


Structure* StructureListBox::selectedStructure()
{
	return (selectedIndex() == constants::NoSelection) ? nullptr : static_cast<StructureListBoxItem*>(mItems[selectedIndex()])->structure;
}


/**
 * Convenience function to get the last item in the list.
 */
StructureListBox::StructureListBoxItem* StructureListBox::last()
{
	return static_cast<StructureListBoxItem*>(mItems.back());
}


/**
 * Draws the FactoryListBox
 */
void StructureListBox::update()
{
	if (!visible()) { return; }
	ListBoxBase::update();

	auto& renderer = Utility<Renderer>::get();

	renderer.clipRect(mRect);

	// ITEMS
	for (std::size_t i = 0; i < mItems.size(); ++i)
	{
		drawItem(renderer, *static_cast<StructureListBoxItem*>(mItems[i]),
			{
				positionX(),
				positionY() + (static_cast<int>(i) * LIST_ITEM_HEIGHT) - static_cast<int>(draw_offset()),
				static_cast<int>(item_width()),
				LIST_ITEM_HEIGHT
			},
			i == selectedIndex());
	}

	renderer.clipRectClear();
}
