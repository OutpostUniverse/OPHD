// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryListBox.h"

#include "../Things/Structures/Factory.h"

#include "../Cache.h"
#include "../Constants.h"
#include "../FontManager.h"


using namespace NAS2D;


const int LIST_ITEM_HEIGHT = 58;
const Image* STRUCTURE_ICONS = nullptr;

static const Font* MAIN_FONT = nullptr;
static const Font* MAIN_FONT_BOLD = nullptr;


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
}


void FactoryListBox::_init()
{
	item_height(LIST_ITEM_HEIGHT);
	STRUCTURE_ICONS = &imageCache.load("ui/structures.png");
	MAIN_FONT = &Utility<FontManager>::get().load(constants::FONT_PRIMARY, 12);
	MAIN_FONT_BOLD = &Utility<FontManager>::get().load(constants::FONT_PRIMARY_BOLD, 12);
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
	
	if (factory->state() == StructureState::Destroyed) { item->icon_slice = {414, 368}; }
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
	for (std::size_t i = 0; i < mItems.size(); ++i)
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
	return (currentSelection() == constants::NO_SELECTION) ? nullptr : static_cast<FactoryListBoxItem*>(mItems[currentSelection()])->factory;
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
			item_width(),
			draw_offset(),
			i == currentSelection());
	}

	renderer.clipRectClear();
}
