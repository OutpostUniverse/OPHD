#include "IconGrid.h"

#include "../Cache.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <limits>


using namespace NAS2D;


const IconGrid::Index IconGrid::NoSelection{std::numeric_limits<Index>::max()};


IconGrid::IconGrid(Delegate selectionChangedHandler, const std::string& filePath, int iconEdgeSize, int margin, bool showTooltip) :
	mSkin{
		imageCache.load("ui/skin/textbox_top_left.png"),
		imageCache.load("ui/skin/textbox_top_middle.png"),
		imageCache.load("ui/skin/textbox_top_right.png"),
		imageCache.load("ui/skin/textbox_middle_left.png"),
		imageCache.load("ui/skin/textbox_middle_middle.png"),
		imageCache.load("ui/skin/textbox_middle_right.png"),
		imageCache.load("ui/skin/textbox_bottom_left.png"),
		imageCache.load("ui/skin/textbox_bottom_middle.png"),
		imageCache.load("ui/skin/textbox_bottom_right.png")
	},
	mFont{Control::getDefaultFont()},
	mIconSheet{imageCache.load(filePath)},
	mShowTooltip{showTooltip},
	mIconSize{iconEdgeSize},
	mIconMargin{margin},
	mSelectionChangedDelegate{selectionChangedHandler}
{
	if (iconEdgeSize <= 0)
	{
		throw std::runtime_error("IconGrid::iconSize must be positive: " + std::to_string(iconEdgeSize));
	}
	if (margin < 0)
	{
		throw std::runtime_error("IconGrid::iconMargin must be non-negative: " + std::to_string(margin));
	}

	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &IconGrid::onMouseDown});
	eventHandler.mouseMotion().connect({this, &IconGrid::onMouseMove});
}


IconGrid::~IconGrid()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &IconGrid::onMouseDown});
	eventHandler.mouseMotion().disconnect({this, &IconGrid::onMouseMove});
}


void IconGrid::addItem(const Item& item)
{
	mIconItemList.push_back(item);

	const auto divisor = mIconSheet.size().x / mIconSize;
	mIconItemList.back().pos = {(item.sheetId % divisor) * mIconSize, (item.sheetId / divisor) * mIconSize};
}


void IconGrid::sort()
{
	const auto iconItemCompare = [](const auto& left, const auto& right){ return left.name < right.name; };
	std::sort(mIconItemList.begin(), mIconItemList.end(), iconItemCompare);
}


void IconGrid::clear()
{
	mIconItemList.clear();
	clearSelection();
}


bool IconGrid::itemExists(const std::string& itemName) const
{
	for (const auto& iconItem : mIconItemList)
	{
		if (iconItem.name == itemName)
		{
			return true;
		}
	}
	return false;
}


void IconGrid::removeItem(const std::string& itemName)
{
	const auto iter = std::find_if(
		mIconItemList.begin(),
		mIconItemList.end(),
		[&itemName](const auto& iconItem) {
			return iconItem.name == itemName;
		}
	);
	if (iter != mIconItemList.end())
	{
		mIconItemList.erase(iter);
		clearSelection();
	}
}


bool IconGrid::itemAvailable(const std::string& itemName) const
{
	for (const auto& iconItem : mIconItemList)
	{
		if (iconItem.name == itemName)
		{
			return iconItem.available;
		}
	}
	return false;
}


void IconGrid::itemAvailable(const std::string& itemName, bool isItemAvailable)
{
	for (auto& iconItem : mIconItemList)
	{
		if (iconItem.name == itemName)
		{
			iconItem.available = isItemAvailable;
			return;
		}
	}
}


void IconGrid::clearSelection()
{
	mHighlightIndex = NoSelection;
	mSelectedIndex = NoSelection;
}


void IconGrid::setSelection(Index newSelection)
{
	mSelectedIndex = (newSelection < mIconItemList.size()) ? newSelection : NoSelection;
}


/**
 * Sets the current selection index to the first item with
 * a matching meta value.
 *
 * \note	If no items in the list contain a matching meta value,
 *			the current selection index will remain unchanged.
 *
 * \remark	IconGrid::Item::meta is initialized to 0 so passing a value
 *			of 0 for IconGrid::Item's that don't use a meta value will
 *			effectively set the index to 0.
 */
void IconGrid::setSelectionByMeta(int selectionMetaValue)
{
	for (Index i = 0; i < mIconItemList.size(); ++i)
	{
		if (mIconItemList[i].meta == selectionMetaValue)
		{
			mSelectedIndex = i;
			return;
		}
	}
}


void IconGrid::incrementSelection()
{
	++mSelectedIndex;
	if (mSelectedIndex >= mIconItemList.size())
	{
		mSelectedIndex = 0;
	}

	if (mIconItemList.empty())
	{
		mSelectedIndex = NoSelection;
	}

	raiseChangedEvent();
}


void IconGrid::decrementSelection()
{
	if (mSelectedIndex == 0)
	{
		mSelectedIndex = mIconItemList.size();
	}
	--mSelectedIndex;

	if (mIconItemList.empty())
	{
		mSelectedIndex = NoSelection;
	}

	raiseChangedEvent();
}


void IconGrid::raiseChangedEvent() const
{
	if (mSelectedIndex != NoSelection)
	{
		mSelectionChangedDelegate(&mIconItemList[mSelectedIndex]);
	}
	else
	{
		mSelectionChangedDelegate(nullptr);
	}
}


void IconGrid::hide()
{
	Control::hide();
	clearSelection();
}


void IconGrid::update()
{
	if (!visible()) { return; }

	auto& renderer = Utility<Renderer>::get();

	mSkin.draw(renderer, mRect);

	if (mGridSizeInIcons.x == 0) { return; }

	for (Index i = 0; i < mIconItemList.size(); ++i)
	{
		const auto iconArea = indexToArea(i);
		const auto highlightColor = mIconItemList[i].available ? NAS2D::Color::White : NAS2D::Color::Red;
		renderer.drawSubImage(mIconSheet, iconArea.position, NAS2D::Rectangle{mIconItemList[i].pos, iconArea.size}, highlightColor);
	}

	if (mSelectedIndex != NoSelection)
	{
		const auto iconArea = indexToArea(mSelectedIndex);
		renderer.drawBox(iconArea, NAS2D::Color{0, 100, 255});
	}

	if (mHighlightIndex != NoSelection)
	{
		const auto iconArea = indexToArea(mHighlightIndex);
		renderer.drawBox(iconArea, NAS2D::Color{0, 180, 0});

		// Name Tooltip
		if (mShowTooltip)
		{
			const auto& highlightedName = mIconItemList[mHighlightIndex].name;
			const auto textMargin = NAS2D::Vector{2, 0};
			const auto textBoxSize = mFont.size(highlightedName) + textMargin * 2;
			const auto textBoxOffset = NAS2D::Vector{0, -textBoxSize.y};
			const auto tooltipRect = NAS2D::Rectangle{iconArea.position + textBoxOffset, textBoxSize};
			renderer.drawBoxFilled(tooltipRect, NAS2D::Color{245, 245, 245});
			renderer.drawBox(tooltipRect, NAS2D::Color{175, 175, 175});
			renderer.drawText(mFont, highlightedName, tooltipRect.position + textMargin, NAS2D::Color::Black);
		}
	}
}


void IconGrid::onResize()
{
	Control::onResize();

	mGridSizeInIcons = (mRect.size - NAS2D::Vector{mIconMargin, mIconMargin} * 2) / (mIconSize + mIconMargin);
}


void IconGrid::onMouseDown(MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible() || !mRect.contains(position)) { return; }
	if (button != MouseButton::Left) { return; }

	const auto iconIndex = positionToIndex(position);
	if (mSelectedIndex != iconIndex)
	{
		mSelectedIndex = iconIndex;
		raiseChangedEvent();
	}
}


void IconGrid::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (!enabled() || !visible()) { return; }

	mHighlightIndex = positionToIndex(position);
}


bool IconGrid::isInGridArea(NAS2D::Point<int> position) const
{
	return !mIconItemList.empty() && NAS2D::Rectangle{mRect.position, mGridSizeInIcons * (mIconSize + mIconMargin)}.contains(position);
}


IconGrid::Index IconGrid::positionToIndex(NAS2D::Point<int> position) const
{
	if (!isInGridArea(position)) { return NoSelection; }
	const auto relativeOffset = position - mRect.position;
	const auto gridOffset = (relativeOffset / (mIconSize + mIconMargin)).to<Index>();
	const auto index = gridOffset.x + (static_cast<Index>(mGridSizeInIcons.x) * gridOffset.y);
	return (index >= mIconItemList.size()) ? NoSelection : index;
}


NAS2D::Point<int> IconGrid::indexToPosition(Index index) const
{
	// Assume a width of a least one grid icon, so we avoid division by 0
	const auto divisor = std::max(mGridSizeInIcons.x, 1);
	const auto linearOffset = static_cast<int>(index);
	const auto offset = NAS2D::Vector{linearOffset % divisor, linearOffset / divisor};
	return mRect.position + NAS2D::Vector{mIconMargin, mIconMargin} + offset * (mIconSize + mIconMargin);
}


NAS2D::Rectangle<int> IconGrid::indexToArea(Index index) const
{
	return NAS2D::Rectangle{indexToPosition(index), {mIconSize, mIconSize}};
}
