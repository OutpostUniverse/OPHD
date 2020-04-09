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


static void drawItem(Renderer& r, FactoryListBox::FactoryListBoxItem& item, float x, float y, float w, float offset, bool highlight)
{
	Factory* f = item.factory;

	const auto& structureColor = structureColorFromIndex(static_cast<std::size_t>(f->state()));
	const auto& structureTextColor = structureTextColorFromIndex(static_cast<std::size_t>(f->state()));

	// draw highlight rect so as not to tint/hue colors of everything else
	if (highlight) { r.drawBoxFilled(x, y - offset, w, LIST_ITEM_HEIGHT, structureColor.red(), structureColor.green(), structureColor.blue(), 75); }

	r.drawBox({x + 2, y + 2 - offset, w - 4, LIST_ITEM_HEIGHT - 4}, structureColor);
	r.drawSubImage(*STRUCTURE_ICONS, x + 8, y + 8 - offset, static_cast<float>(item.icon_slice.x()), static_cast<float>(item.icon_slice.y()), 46.0f, 46.0f, 255, 255, 255, structureColor.alpha());

	r.drawText(*MAIN_FONT_BOLD, f->name(), {x + 64, ((y + 29) - MAIN_FONT_BOLD->height() / 2) - offset}, structureTextColor);

	r.drawText(*MAIN_FONT, productDescription(f->productType()), {x + w - 112, ((y + 19) - MAIN_FONT_BOLD->height() / 2) - offset}, structureTextColor);
	
	// PROGRESS BAR
	float percentage = (f->productType() == ProductType::PRODUCT_NONE) ? 0.0f : (f->productionTurnsCompleted() / f->productionTurnsToComplete());
	drawBasicProgressBar(x + w - 112.0f, y + 30.0f - offset, 105.0f, 11.0f, percentage, 2.0f);
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
	if (item->Text == constants::SURFACE_FACTORY) { item->icon_slice = {0, 46}; }
	else if (item->Text == constants::UNDERGROUND_FACTORY) { item->icon_slice = {138, 276}; }
	else if (item->Text == constants::SEED_FACTORY) { item->icon_slice = {460, 368}; }
	
	if (factory->state() == Structure::StructureState::DESTROYED) { item->icon_slice = {414, 368}; }
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
 * \param f	Pointer to a Factory object. Safe to pass \c nullptr.
 */
void FactoryListBox::currentSelection(Factory* f)
{
	if (mItems.empty() || f == nullptr) { return; }
	for (size_t i = 0; i < mItems.size(); ++i)
	{
		FactoryListBoxItem* item = static_cast<FactoryListBoxItem*>(mItems[i]);
		if (item->factory == f)
		{
			setSelection(static_cast<int>(i));
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
		drawItem(r, *static_cast<FactoryListBoxItem*>(mItems[i]),
			positionX(),
			positionY() + (i * LIST_ITEM_HEIGHT),
			static_cast<float>(item_width()),
			static_cast<float>(draw_offset()),
			i == ListBoxBase::currentSelection());
	}

	r.clipRectClear();
}
