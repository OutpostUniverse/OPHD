#include "StructureListBox.h"

#include "StructureColor.h"

#include "../MapObjects/Structure.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <stdexcept>


using namespace NAS2D;


StructureListBox::StructureListBox() :
	ListBoxBase{
		fontCache.load(constants::FontPrimary, 12),
		fontCache.load(constants::FontPrimaryBold, 12)
	}
{
	itemHeight(30);
}


std::size_t StructureListBox::count() const
{
	return mItems.size();
}


/**
 * Adds a Factory to the FactoryListBox.
 *
 * Specialized version of the default addItem(ListBoxItem*) function.
 */
void StructureListBox::addItem(Structure* structure, std::string stateDescription)
{
	for (const auto& item : mItems)
	{
		if (item.structure == structure)
		{
			throw std::runtime_error("StructureListBox::addItem(): Can't add structure multiple times");
		}
	}

	add(structure, std::move(stateDescription));
}


/**
 * Sets the current selection.
 *
 * \param structure		Pointer to a Structure object. Safe to pass \c nullptr.
 */
void StructureListBox::setSelected(const Structure* structure)
{
	if (structure == nullptr)
	{
		setSelection(NoSelection);
		return;
	}

	for (std::size_t index = 0; index < mItems.size(); ++index)
	{
		const auto& item = getItem(index);
		if (item.structure == structure)
		{
			setSelection(index);
			return;
		}
	}

	setSelection(NoSelection);
}


Structure* StructureListBox::selectedStructure()
{
	return !isItemSelected() ? nullptr : getItem(selectedIndex()).structure;
}


/**
 * Convenience function to get the last item in the list.
 */
StructureListBox::StructureListBoxItem* StructureListBox::last()
{
	return &mItems.back();
}


void StructureListBox::clear()
{
	mItems.clear();
	ListBoxBase::clear();
}


void StructureListBox::add(Structure* s, std::string stateDescription)
{
	mItems.emplace_back(StructureListBoxItem{s->name(), s, std::move(stateDescription)});
	updateScrollLayout();
}


const StructureListBox::StructureListBoxItem& StructureListBox::getItem(std::size_t index) const
{
	return mItems[index];
}


NAS2D::Color StructureListBox::itemBorderColor(std::size_t index) const
{
	const auto& item = getItem(index);
	const auto structureState = item.structure->state();
	return structureColorFromIndex(structureState);
}


void StructureListBox::drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const
{
	const auto& item = getItem(index);
	const auto structureState = item.structure->state();
	const auto& structureTextColor = structureTextColorFromIndex(structureState);

	const auto yOffset = 15 - mFontBold.height() / 2;
	renderer.drawText(mFontBold, item.text, drawArea.position + NAS2D::Vector{5, yOffset}, structureTextColor);
	renderer.drawText(mFont, item.stateDescription, drawArea.crossXPoint() + NAS2D::Vector{-mFont.width(item.stateDescription) - 5, yOffset}, structureTextColor);
}
