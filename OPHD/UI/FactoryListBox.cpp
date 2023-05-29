#include "FactoryListBox.h"

#include "../MapObjects/Structures/Factory.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../ProductCatalogue.h"

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


static void drawItem(Renderer& renderer, FactoryListBox::FactoryListBoxItem& item, NAS2D::Rectangle<int> rect, bool highlight)
{
	Factory* f = item.factory;

	const auto productType = f->productType();
	const auto factoryState = f->state();

	const auto& structureColor = structureColorFromIndex(factoryState);
	const auto& structureTextColor = structureTextColorFromIndex(factoryState);

	// draw highlight rect so as not to tint/hue colors of everything else
	if (highlight) { renderer.drawBoxFilled(rect, structureColor.alphaFade(75)); }
	renderer.drawBox(rect.inset(2), structureColor);

	const auto subImageRect = NAS2D::Rectangle{item.icon_slice, {46, 46}};
	renderer.drawSubImage(*STRUCTURE_ICONS, rect.position + NAS2D::Vector{8, 8}, subImageRect, NAS2D::Color::White.alphaFade(structureColor.alpha));
	renderer.drawText(*MAIN_FONT_BOLD, f->name(), rect.position + NAS2D::Vector{64, 29 - MAIN_FONT_BOLD->height() / 2}, structureTextColor);
	if (productType != ProductType::PRODUCT_NONE)
	{
		renderer.drawText(*MAIN_FONT, ProductCatalogue::get(productType).Name, rect.crossXPoint() + NAS2D::Vector{-112, 19 - MAIN_FONT_BOLD->height() / 2}, structureTextColor);
		drawProgressBar(
			f->productionTurnsCompleted(),
			f->productionTurnsToComplete(),
			NAS2D::Rectangle<int>::Create(rect.crossXPoint() + NAS2D::Vector{-112, 30}, NAS2D::Vector{105, 11}),
			2
		);
	}
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
			{
				{positionX(),
				positionY() + (static_cast<int>(i) * LIST_ITEM_HEIGHT) - static_cast<int>(draw_offset())},
				{static_cast<int>(item_width()),
				LIST_ITEM_HEIGHT}
			},
			i == selectedIndex());
	}

	renderer.clipRectClear();
}
