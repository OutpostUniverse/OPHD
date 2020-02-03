// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "StructureListBox.h"

#include "../Things/Structures/Structure.h"

#include "../Constants.h"
#include "../FontManager.h"

using namespace NAS2D;


const int LIST_ITEM_HEIGHT = 30;

static Font* MAIN_FONT = nullptr;
static Font* MAIN_FONT_BOLD = nullptr;

static Color*	STRUCTURE_COLOR;
static Color*	STRUCTURE_TEXT_COLOR;


static void drawItem(Renderer& r, StructureListBox::StructureListBoxItem& item, float x, float y, float w, float offset, bool highlight)
{
	Structure* _st = item.structure;

	STRUCTURE_COLOR = &structureColorFromIndex(_st->state());
	STRUCTURE_TEXT_COLOR = &structureTextColorFromIndex(_st->state());

	// draw highlight rect so as not to tint/hue colors of everything else
	if (highlight) { r.drawBoxFilled(x, y - offset, w, LIST_ITEM_HEIGHT, STRUCTURE_COLOR->red(), STRUCTURE_COLOR->green(), STRUCTURE_COLOR->blue(), 75); }

	r.drawBox(x + 2, y + 2 - offset, w - 4, LIST_ITEM_HEIGHT - 4, STRUCTURE_COLOR->red(), STRUCTURE_COLOR->green(), STRUCTURE_COLOR->blue(), STRUCTURE_COLOR->alpha());

	r.drawText(*MAIN_FONT_BOLD, item.Text, x + 5, ((y + 15) - MAIN_FONT_BOLD->height() / 2) - offset,
				STRUCTURE_TEXT_COLOR->red(), STRUCTURE_TEXT_COLOR->green(), STRUCTURE_TEXT_COLOR->blue(), STRUCTURE_TEXT_COLOR->alpha());

	r.drawText(*MAIN_FONT, item.structureState, x + w - MAIN_FONT->width(item.structureState) - 5, ((y + 15) - MAIN_FONT_BOLD->height() / 2) - offset,
				STRUCTURE_TEXT_COLOR->red(), STRUCTURE_TEXT_COLOR->green(), STRUCTURE_TEXT_COLOR->blue(), STRUCTURE_TEXT_COLOR->alpha());
}


StructureListBox::StructureListBoxItem::StructureListBoxItem(Structure* _st) : ListBoxItem(_st->name()), structure(_st), structureState(""), colorIndex(_st->state()) { Text = _st->name(); }
StructureListBox::StructureListBoxItem::~StructureListBoxItem() {}


/**
 * C'tor
 */
StructureListBox::StructureListBox()
{
	_init();
}


/**
 * D'tor
 */
StructureListBox::~StructureListBox()
{}


void StructureListBox::_init()
{
	item_height(LIST_ITEM_HEIGHT);
	MAIN_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 12);
	MAIN_FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 12);
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

	for (size_t i = 0; i < mItems.size(); ++i)
	{
		StructureListBoxItem* item = static_cast<StructureListBoxItem*>(mItems[i]);
		if (item->structure == structure)
		{
			setSelection(static_cast<int>(i));
			return;
		}
	}
}


Structure* StructureListBox::selectedStructure()
{
	return (ListBoxBase::currentSelection() == constants::NO_SELECTION) ? nullptr : static_cast<StructureListBoxItem*>(mItems[ListBoxBase::currentSelection()])->structure;
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

	Renderer& r = Utility<Renderer>::get();

	r.clipRect(rect());

	// ITEMS
	for (size_t i = 0; i < mItems.size(); ++i)
	{
		drawItem(r, *static_cast<StructureListBoxItem*>(mItems[i]),
			positionX(),
			positionY() + (i * LIST_ITEM_HEIGHT),
			static_cast<float>(item_width()),
			static_cast<float>(draw_offset()),
			i == ListBoxBase::currentSelection());
	}

	r.clipRectClear();
}
