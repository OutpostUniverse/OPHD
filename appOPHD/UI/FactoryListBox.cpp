#include "FactoryListBox.h"

#include "ProgressBar.h"
#include "StructureColor.h"

#include "../MapObjects/Structures/Factory.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../ProductCatalogue.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <stdexcept>


using namespace NAS2D;


FactoryListBox::FactoryListBox() :
	ListBoxBase{
		fontCache.load(constants::FONT_PRIMARY, 12),
		fontCache.load(constants::FONT_PRIMARY_BOLD, 12)
	},
	mStructureIcons{imageCache.load("ui/structures.png")}
{
	itemHeight(58);
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
	return !isItemSelected() ? nullptr : static_cast<FactoryListBoxItem*>(mItems[selectedIndex()])->factory;
}


void FactoryListBox::drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index, bool isSelected) const
{
	const auto& item = *static_cast<FactoryListBoxItem*>(mItems[index]);
	Factory* f = item.factory;

	const auto productType = f->productType();
	const auto factoryState = f->state();

	const auto& structureColor = structureColorFromIndex(factoryState);
	const auto& structureTextColor = structureTextColorFromIndex(factoryState);

	// draw highlight rect so as not to tint/hue colors of everything else
	if (isSelected) { renderer.drawBoxFilled(drawArea, structureColor.alphaFade(75)); }
	renderer.drawBox(drawArea.inset(2), structureColor);

	const auto subImageRect = NAS2D::Rectangle{item.icon_slice, {46, 46}};
	renderer.drawSubImage(mStructureIcons, drawArea.position + NAS2D::Vector{8, 8}, subImageRect, NAS2D::Color::White.alphaFade(structureColor.alpha));
	renderer.drawText(mFontBold, f->name(), drawArea.position + NAS2D::Vector{64, 29 - mFontBold.height() / 2}, structureTextColor);
	if (productType != ProductType::PRODUCT_NONE)
	{
		renderer.drawText(mFont, ProductCatalogue::get(productType).Name, drawArea.crossXPoint() + NAS2D::Vector{-112, 19 - mFontBold.height() / 2}, structureTextColor);
		drawProgressBar(
			f->productionTurnsCompleted(),
			f->productionTurnsToComplete(),
			NAS2D::Rectangle{drawArea.crossXPoint() + NAS2D::Vector{-112, 30}, {105, 11}},
			2
		);
	}
}
