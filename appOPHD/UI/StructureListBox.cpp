#include "StructureListBox.h"

#include "StructureColor.h"

#include "../MapObjects/Structure.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <stdexcept>


using namespace NAS2D;


StructureListBox::StructureListBoxItem::StructureListBoxItem(Structure* s) :
	ListBoxItem{s->name()},
	structure{s},
	structureState{""},
	colorIndex{s->state()}
{}


StructureListBox::StructureListBox() :
	ListBoxBase{
		fontCache.load(constants::FONT_PRIMARY, 12),
		fontCache.load(constants::FONT_PRIMARY_BOLD, 12)
	}
{
	itemHeight(30);
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
	return !isItemSelected() ? nullptr : static_cast<StructureListBoxItem*>(mItems[selectedIndex()])->structure;
}


/**
 * Convenience function to get the last item in the list.
 */
StructureListBox::StructureListBoxItem* StructureListBox::last()
{
	return static_cast<StructureListBoxItem*>(mItems.back());
}


NAS2D::Color StructureListBox::itemBorderColor(std::size_t index) const
{
	const auto& item = *static_cast<StructureListBoxItem*>(mItems[index]);
	const auto structureState = item.structure->state();
	return structureColorFromIndex(structureState);
}


void StructureListBox::drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index, bool isSelected) const
{
	const auto& item = *static_cast<StructureListBoxItem*>(mItems[index]);
	const auto structureState = item.structure->state();
	const auto& structureTextColor = structureTextColorFromIndex(structureState);

	// draw highlight rect so as not to tint/hue colors of everything else
	const auto& borderColor = itemBorderColor(index);
	if (isSelected) { renderer.drawBoxFilled(drawArea, borderColor.alphaFade(75)); }
	renderer.drawBox(drawArea.inset(2), borderColor);

	const auto yOffset = 15 - mFontBold.height() / 2;
	renderer.drawText(mFontBold, item.text, drawArea.position + NAS2D::Vector{5, yOffset}, structureTextColor);
	renderer.drawText(mFont, item.structureState, drawArea.crossXPoint() + NAS2D::Vector{-mFont.width(item.structureState) - 5, yOffset}, structureTextColor);
}
