// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryListBox.h"

#include "../Constants.h"
#include "../FontManager.h"


using namespace NAS2D;


const int LIST_ITEM_HEIGHT = 58;
Image* STRUCTURE_ICONS = nullptr;

static Font* MAIN_FONT = nullptr;
static Font* MAIN_FONT_BOLD = nullptr;



std::map<Structure::StructureState, Color_4ub> StructureColorTable
{
	{ Structure::UNDER_CONSTRUCTION,	Color_4ub(150, 150, 150, 100) },
	{ Structure::OPERATIONAL,			Color_4ub(0, 185, 0, 255) },
	{ Structure::IDLE,					Color_4ub(0, 185, 0, 100) },
	{ Structure::DISABLED,				Color_4ub(220, 0, 0, 255) },
	{ Structure::DESTROYED,				Color_4ub(220, 0, 0, 255) }
};


std::map<Structure::StructureState, Color_4ub> StructureTextColorTable
{
	{ Structure::UNDER_CONSTRUCTION,	Color_4ub(185, 185, 185, 100) },
	{ Structure::OPERATIONAL,			Color_4ub(0, 185, 0, 255) },
	{ Structure::IDLE,					Color_4ub(0, 185, 0, 100) },
	{ Structure::DISABLED,				Color_4ub(220, 0, 0, 255) },
	{ Structure::DESTROYED,				Color_4ub(220, 0, 0, 255) }
};



static Color_4ub*	STRUCTURE_COLOR;
static Color_4ub*	STRUCTURE_TEXT_COLOR;


static void drawItem(Renderer& r, FactoryListBox::FactoryListBoxItem& item, int x, int y, int w, int offset, bool highlight)
{
	Factory* f = item.factory;

	STRUCTURE_COLOR = &StructureColorTable[f->state()];
	STRUCTURE_TEXT_COLOR = &StructureTextColorTable[f->state()];

	// draw highlight rect so as not to tint/hue colors of everything else
	if (highlight) { r.drawBoxFilled(x, y - offset, w, LIST_ITEM_HEIGHT, STRUCTURE_COLOR->red(), STRUCTURE_COLOR->green(), STRUCTURE_COLOR->blue(), 75); }

	r.drawBox(x + 2, y + 2 - offset, w - 4, LIST_ITEM_HEIGHT - 4, STRUCTURE_COLOR->red(), STRUCTURE_COLOR->green(), STRUCTURE_COLOR->blue(), STRUCTURE_COLOR->alpha());
	r.drawSubImage(*STRUCTURE_ICONS, x + 8, y + 8 - offset, item.icon_slice.x(), item.icon_slice.y(), 46, 46, 255, 255, 255, STRUCTURE_COLOR->alpha());

	r.drawText(*MAIN_FONT_BOLD, f->name(), x + 64, ((y + 29) - MAIN_FONT_BOLD->height() / 2) - offset,
				STRUCTURE_TEXT_COLOR->red(), STRUCTURE_TEXT_COLOR->green(), STRUCTURE_TEXT_COLOR->blue(), STRUCTURE_TEXT_COLOR->alpha());

	r.drawText(*MAIN_FONT, productDescription(f->productType()), x + w - 112, ((y + 19) - MAIN_FONT_BOLD->height() / 2) - offset,
				STRUCTURE_TEXT_COLOR->red(), STRUCTURE_TEXT_COLOR->green(), STRUCTURE_TEXT_COLOR->blue(), STRUCTURE_TEXT_COLOR->alpha());
	
	// PROGRESS BAR
	float percentage = (f->productType() == PRODUCT_NONE) ? 0.0f : (f->productionTurnsCompleted() / f->productionTurnsToComplete());
	drawBasicProgressBar(x + w - 112, y + 30 - offset, 105, 11, percentage, 2);
}


/**
 * C'tor
 */
FactoryListBox::FactoryListBox()
{
	_init();
}


/**
 * D'tor
 */
FactoryListBox::~FactoryListBox()
{
	delete STRUCTURE_ICONS;
}


void FactoryListBox::_init()
{
	item_height(LIST_ITEM_HEIGHT);
	STRUCTURE_ICONS = new Image("ui/structures.png");
	MAIN_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 12);
	MAIN_FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 12);
}


/**
 * Adds a Factory to the FactoryListBox.
 * 
 * Specialized version of the default addItem(ListBoxItem*) function.
 */
void FactoryListBox::addItem(Factory* factory)
{
	/// \fixme	Could be much more elegant via a lambda expression
	for (auto item : mItems)
	{
		if (static_cast<FactoryListBoxItem*>(item)->factory == factory)
		{
			std::cout << "FactoryListBox::addItem(): annoying bug, fix it." << std::endl;
			return;
		}
	}

	mItems.push_back(new FactoryListBoxItem(factory));

	/// \fixme super sloppy
	FactoryListBoxItem* item = static_cast<FactoryListBoxItem*>(mItems.back());
	item->Text = factory->name();
	if (item->Text == constants::SURFACE_FACTORY) { item->icon_slice(0, 46); }
	else if (item->Text == constants::UNDERGROUND_FACTORY) { item->icon_slice(138, 276); }
	else if (item->Text == constants::SEED_FACTORY) { item->icon_slice(460, 368); }
	
	if (factory->state() == Structure::DESTROYED) { item->icon_slice(414, 368); }
	_update_item_display();
}


/**
 * Removes a Factory from the FactoryListBox.
 * 
 * Specialized version of the default addItem(ListBoxItem*) function.
 */
void FactoryListBox::removeItem(Factory* factory)
{
	for (auto it = mItems.begin(); it != mItems.end(); ++it)
	{
		if (static_cast<FactoryListBoxItem*>(*it)->factory == factory)
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
 * \param f	Pointer to a Factory object. Save to pass \c nullptr.
 */
void FactoryListBox::currentSelection(Factory* f)
{
	if (mItems.empty() || f == nullptr) { return; }
	for (size_t i = 0; i < mItems.size(); ++i)
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
	return (ListBoxBase::currentSelection() == constants::NO_SELECTION) ? nullptr : static_cast<FactoryListBoxItem*>(mItems[ListBoxBase::currentSelection()])->factory;
}


/**
 * Draws the FactoryListBox
 */
void FactoryListBox::update()
{
	if (!visible()) { return; }

	ListBoxBase::update();

	Renderer& r = Utility<Renderer>::get();

	r.clipRect(rect());

	// ITEMS
	for (size_t i = 0; i < mItems.size(); ++i)
	{
		drawItem(r, *static_cast<FactoryListBoxItem*>(mItems[i]), positionX(), positionY() + (i * LIST_ITEM_HEIGHT), item_width(), draw_offset(), i == ListBoxBase::currentSelection());
	}

	r.clipRectClear();
}
