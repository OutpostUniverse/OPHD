// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "StructureListBox.h"

#include "../Things/Structures/Structure.h"

#include "../Constants.h"
#include "../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


const int LIST_ITEM_HEIGHT = 30;

static const Font* MAIN_FONT = nullptr;
static const Font* MAIN_FONT_BOLD = nullptr;


static void drawItem(Renderer& renderer, StructureListBox::StructureListBoxItem& item, int x, int y, int w, int offset, bool highlight)
{
	Structure* structure = item.structure;

	const auto& structureColor = structureColorFromIndex(structure->state());
	const auto& structureTextColor = structureTextColorFromIndex(structure->state());
	const auto highlightColor = NAS2D::Color{structureColor.red, structureColor.green, structureColor.blue, 75};

	// draw highlight rect so as not to tint/hue colors of everything else
	if (highlight) { renderer.drawBoxFilled(NAS2D::Rectangle{x, y - offset, w, LIST_ITEM_HEIGHT}, highlightColor); }

	renderer.drawBox(NAS2D::Rectangle{x + 2, y + 2 - offset, w - 4, LIST_ITEM_HEIGHT - 4}, structureColor);

	renderer.drawText(*MAIN_FONT_BOLD, item.Text, NAS2D::Point{x + 5, ((y + 15) - MAIN_FONT_BOLD->height() / 2) - offset}, structureTextColor);

	renderer.drawText(*MAIN_FONT, item.structureState, NAS2D::Point{x + w - MAIN_FONT->width(item.structureState) - 5, ((y + 15) - MAIN_FONT_BOLD->height() / 2) - offset}, structureTextColor);
}


StructureListBox::StructureListBoxItem::StructureListBoxItem(Structure* s) :
	ListBoxItem(s->name()),
	structure(s),
	structureState(""),
	colorIndex(s->state())
{}


/**
 * C'tor
 */
StructureListBox::StructureListBox()
{
	_init();
}


void StructureListBox::_init()
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
	/// \fixme	Could be much more elegant via a lambda expression
	for (auto item : mItems)
	{
		if (static_cast<StructureListBoxItem*>(item)->structure == structure)
		{
			std::cout << "StructureListBox::addItem(): annoying bug, fix it." << std::endl;
			return;
		}
	}

	mItems.push_back(new StructureListBoxItem(structure));
	_update_item_display();
}


/**
 * Removes a Factory from the FactoryListBox.
 * 
 * Specialized version of the default addItem(ListBoxItem*) function.
 */
void StructureListBox::removeItem(Structure* structure)
{
	for (auto it = mItems.begin(); it != mItems.end(); ++it)
	{
		if (static_cast<StructureListBoxItem*>(*it)->structure == structure)
		{
			mItems.erase(it);
			_update_item_display();
			clearSelection();
			return;
		}
	}
}


/**
 * Sets the current selection.
 * 
 * \param structure		Pointer to a Structure object. Save to pass \c nullptr.
 */
void StructureListBox::currentSelection(Structure* structure)
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
	return (currentSelection() == constants::NO_SELECTION) ? nullptr : static_cast<StructureListBoxItem*>(mItems[currentSelection()])->structure;
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
			positionX(),
			positionY() + (static_cast<int>(i) * LIST_ITEM_HEIGHT),
			static_cast<int>(item_width()),
			static_cast<int>(draw_offset()),
			i == currentSelection());
	}

	renderer.clipRectClear();
}
