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
	for (const auto& item : mItems)
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
		const auto& item = getItem(i);
		if (item.factory == f)
		{
			setSelection(i);
			return;
		}
	}
}


Factory* FactoryListBox::selectedFactory()
{
	return !isItemSelected() ? nullptr : getItem(selectedIndex()).factory;
}


const FactoryListBox::FactoryListBoxItem& FactoryListBox::getItem(std::size_t index) const
{
	return *static_cast<FactoryListBoxItem*>(mItems[index]);
}


NAS2D::Color FactoryListBox::itemBorderColor(std::size_t index) const
{
	const auto& item = getItem(index);
	const Factory& factory = *item.factory;
	const auto factoryState = factory.state();
	return structureColorFromIndex(factoryState);
}


void FactoryListBox::drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const
{
	const auto& item = getItem(index);
	const Factory& factory = *item.factory;
	const auto productType = factory.productType();
	const auto factoryState = factory.state();
	const auto& structureTextColor = structureTextColorFromIndex(factoryState);
	const auto& borderColor = itemBorderColor(index);

	const auto subImageRect = NAS2D::Rectangle{item.icon_slice, {46, 46}};
	renderer.drawSubImage(mStructureIcons, drawArea.position + NAS2D::Vector{8, 8}, subImageRect, NAS2D::Color::White.alphaFade(borderColor.alpha));
	renderer.drawText(mFontBold, factory.name(), drawArea.position + NAS2D::Vector{64, 29 - mFontBold.height() / 2}, structureTextColor);
	if (productType != ProductType::PRODUCT_NONE)
	{
		renderer.drawText(mFont, ProductCatalogue::get(productType).Name, drawArea.crossXPoint() + NAS2D::Vector{-112, 19 - mFontBold.height() / 2}, structureTextColor);
		drawProgressBar(
			factory.productionTurnsCompleted(),
			factory.productionTurnsToComplete(),
			NAS2D::Rectangle{drawArea.crossXPoint() + NAS2D::Vector{-112, 30}, {105, 11}},
			2
		);
	}
}
