#include "FactoryListBox.h"

#include "ProgressBar.h"
#include "StructureColor.h"

#include "../MapObjects/StructureState.h"
#include "../MapObjects/Structures/Factory.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include <libOPHD/ProductCatalog.h>

#include <NAS2D/Renderer/Renderer.h>

#include <stdexcept>


FactoryListBox::FactoryListBox(SelectionChangedDelegate selectionChangedHandler) :
	ListBoxBase{{0, 58}, selectionChangedHandler},
	mFont{fontCache.load(constants::FontPrimary, 12)},
	mFontBold{fontCache.load(constants::FontPrimaryBold, 12)},
	mStructureIcons{imageCache.load("ui/structures.png")}
{
}


void FactoryListBox::add(std::string textDescription, Factory* newFactory, NAS2D::Point<int> iconPosition)
{
	mItems.emplace_back(FactoryListBoxItem{textDescription, newFactory, iconPosition});
	updateScrollLayout();
}


std::size_t FactoryListBox::count() const
{
	return mItems.size();
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
		if (item.factory == factory)
		{
			throw std::runtime_error("FactoryListBox::addItem(): Can't add factory multiple times");
		}
	}

	const auto& text = factory->name();
	const auto iconPosition = (factory->state() == StructureState::Destroyed) ? NAS2D::Point<int>{414, 368} :
		(text == constants::UndergroundFactory) ? NAS2D::Point<int>{138, 276} :
		(text == constants::SeedFactory) ? NAS2D::Point<int>{460, 368} :
		NAS2D::Point<int>{0, 46}; // Surface factory
	add(text, factory, iconPosition);
}


/**
 * Sets the current selection.
 *
 * \param factory	Pointer to a Factory object. Safe to pass \c nullptr.
 */
void FactoryListBox::setSelected(Factory* factory)
{
	if (mItems.empty() || factory == nullptr) { return; }
	for (std::size_t index = 0; index < mItems.size(); ++index)
	{
		const auto& item = getItem(index);
		if (item.factory == factory)
		{
			selectedIndex(index);
			return;
		}
	}
}


Factory* FactoryListBox::selectedFactory()
{
	return !isItemSelected() ? nullptr : getItem(selectedIndex()).factory;
}


void FactoryListBox::clear()
{
	mItems.clear();
	ListBoxBase::clear();
}


const FactoryListBox::FactoryListBoxItem& FactoryListBox::getItem(std::size_t index) const
{
	return mItems[index];
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

	// Draw border
	renderer.drawBox(drawArea.inset(1), borderColor);

	const auto subImageRect = NAS2D::Rectangle{item.icon_slice, {46, 46}};
	renderer.drawSubImage(mStructureIcons, drawArea.position + NAS2D::Vector{8, 8}, subImageRect, NAS2D::Color::White.alphaFade(borderColor.alpha));
	renderer.drawText(mFontBold, factory.name(), drawArea.position + NAS2D::Vector{64, 29 - mFontBold.height() / 2}, structureTextColor);
	if (productType != ProductType::PRODUCT_NONE)
	{
		renderer.drawText(mFont, ProductCatalog::get(productType).name, drawArea.crossXPoint() + NAS2D::Vector{-112, 19 - mFontBold.height() / 2}, structureTextColor);
		drawProgressBar(
			factory.productionTurnsCompleted(),
			factory.productionTurnsToComplete(),
			NAS2D::Rectangle{drawArea.crossXPoint() + NAS2D::Vector{-112, 30}, {105, 11}},
			2
		);
	}
}
