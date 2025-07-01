#include "ListBoxBase.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Point.h>

#include <algorithm>
#include <limits>


const std::size_t ListBoxBase::NoSelection{std::numeric_limits<std::size_t>::max()};


ListBoxBase::ListBoxBase(NAS2D::Vector<int> itemSize, SelectionChangedDelegate selectionChangedHandler) :
	mScrollBar{ScrollBar::ScrollBarType::Vertical, itemSize.y, {this, &ListBoxBase::onSlideChange}},
	mItemSize{itemSize},
	mSelectionChangedHandler{selectionChangedHandler}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().connect({this, &ListBoxBase::onMouseWheel});
	eventHandler.mouseButtonDown().connect({this, &ListBoxBase::onMouseDown});
	eventHandler.mouseMotion().connect({this, &ListBoxBase::onMouseMove});
}


ListBoxBase::~ListBoxBase()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().disconnect({this, &ListBoxBase::onMouseWheel});
	eventHandler.mouseButtonDown().disconnect({this, &ListBoxBase::onMouseDown});
	eventHandler.mouseMotion().disconnect({this, &ListBoxBase::onMouseMove});
}


/**
 * True if no items are in the list.
 */
bool ListBoxBase::isEmpty() const
{
	return count() == 0;
}


/**
 * Index of the current mouse hover highlight.
 */
std::size_t ListBoxBase::currentHighlight() const
{
	return mHighlightIndex;
}


/**
 * Index of the current selection.
 */
std::size_t ListBoxBase::selectedIndex() const
{
	return mSelectedIndex;
}


bool ListBoxBase::isItemSelected() const
{
	return mSelectedIndex != NoSelection;
}


/**
 * Sets the current selection index.
 *
 * \note	Out of range selection indicies will set the ListBoxBase to no selection.
 */
void ListBoxBase::setSelection(std::size_t selection)
{
	mSelectedIndex = (selection < count()) ? selection : NoSelection;
	if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
}


/**
 * Clears the current selection.
 */
void ListBoxBase::clearSelected()
{
	mSelectedIndex = NoSelection;
	if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
}


/**
 * Clears all items from the list.
 */
void ListBoxBase::clear()
{
	mSelectedIndex = NoSelection;
	mHighlightIndex = NoSelection;
	updateScrollLayout();
}


/**
 * Updates values required for properly displaying list items.
 */
void ListBoxBase::updateScrollLayout()
{
	mItemSize.x = mRect.size.x;

	// Account for border around control
	const auto scrollArea = mRect.inset(1);

	const auto neededDisplaySize = mItemSize.y * static_cast<int>(count());
	if (neededDisplaySize > mRect.size.y)
	{
		mScrollBar.size({14, scrollArea.size.y});
		mScrollBar.position({scrollArea.position.x + scrollArea.size.x - mScrollBar.size().x, scrollArea.position.y});
		mScrollBar.max(neededDisplaySize - mRect.size.y);
		mScrollOffsetInPixels = mScrollBar.value();
		mItemSize.x -= mScrollBar.size().x;
		mScrollBar.visible(true);
	}
	else
	{
		mScrollOffsetInPixels = 0;
		mScrollBar.max(0);
		mScrollBar.visible(false);
	}
}


void ListBoxBase::onVisibilityChange(bool visible)
{
	Control::onVisibilityChange(visible);

	updateScrollLayout();
}


/**
 * Resized event handler.
 */
void ListBoxBase::onResize()
{
	updateScrollLayout();
}


/**
 * ScrollBar changed event handler.
 */
void ListBoxBase::onSlideChange(int /*newPosition*/)
{
	updateScrollLayout();
}


void ListBoxBase::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!visible() || !enabled()) { return; }

	if (isEmpty() || button == NAS2D::MouseButton::Middle) { return; }

	if (button == NAS2D::MouseButton::Right && mRect.contains(position))
	{
		clearSelected();
		return;
	}

	// A few basic checks
	if (!area().contains(position) || mHighlightIndex == NoSelection) { return; }
	if (mScrollBar.visible() && mScrollBar.area().contains(position)) { return; }
	if (mHighlightIndex >= count()) { return; }

	setSelection(mHighlightIndex);
}


void ListBoxBase::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (!visible() || isEmpty()) { return; }

	mHasFocus = area().contains(position);

	// Ignore mouse motion events if the pointer isn't within the menu rect.
	if (!mHasFocus)
	{
		mHighlightIndex = NoSelection;
		return;
	}

	// if the mouse is on the scroll bar then the scroll bar should handle that
	if (mScrollBar.visible() && mScrollBar.area().contains(position))
	{
		mHighlightIndex = NoSelection;
		return;
	}

	const auto scrollRelativeY = mScrollOffsetInPixels + position.y - this->position().y;
	const auto index = static_cast<std::size_t>(scrollRelativeY / mItemSize.y);
	mHighlightIndex = (index < count()) ? index : NoSelection;
}


void ListBoxBase::onMouseWheel(NAS2D::Vector<int> scrollAmount)
{
	if (!visible() || !hasFocus() || isEmpty()) { return; }

	mScrollBar.changeValue((scrollAmount.y < 0 ? mItemSize.y : -mItemSize.y));
}


NAS2D::Point<int> ListBoxBase::itemDrawPosition(std::size_t index) const
{
	return position() + NAS2D::Vector{0, static_cast<int>(index) * mItemSize.y - mScrollOffsetInPixels};
}


NAS2D::Rectangle<int> ListBoxBase::itemDrawArea(std::size_t index) const
{
	return {itemDrawPosition(index), mItemSize};
}


NAS2D::Color ListBoxBase::itemBorderColor(std::size_t /*index*/) const
{
	return {0, 185, 0};
}


/**
 * Draws the ListBox
 */
void ListBoxBase::update()
{
	if (!visible()) { return; }
	mScrollBar.update();
	draw();
}


void ListBoxBase::drawItems() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// Determine visible items and draw them
	const auto lineHeight = mItemSize.y;
	const auto firstVisibleIndex = static_cast<std::size_t>(mScrollOffsetInPixels / lineHeight);
	const auto firstInvisibleIndex = static_cast<std::size_t>((mScrollOffsetInPixels + mRect.inset(1).size.y + (lineHeight - 1)) / lineHeight);
	const auto endVisibleIndex = std::min(firstInvisibleIndex, count());
	for (std::size_t index = firstVisibleIndex; index < endVisibleIndex; ++index)
	{
		const auto drawArea = itemDrawArea(index);
		const auto& borderColor = itemBorderColor(index);
		if (index == selectedIndex())
		{
			// Draw background highlight (drawn first to avoid tinting everything else)
			renderer.drawBoxFilled(drawArea, borderColor.alphaFade(75));
		}
		// Draw border
		renderer.drawBox(drawArea.inset(2), borderColor);

		drawItem(renderer, drawArea, index);
	}
}


void ListBoxBase::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// CONTROL EXTENTS
	const auto backgroundRect = NAS2D::Rectangle<int>{{mRect.position.x, mRect.position.y}, {mItemSize.x, mRect.size.y}};
	renderer.drawBoxFilled(backgroundRect, NAS2D::Color::Black);
	renderer.drawBox(mRect, (hasFocus() ? NAS2D::Color{0, 185, 0} : NAS2D::Color{75, 75, 75}));

	renderer.clipRect(mRect.inset(1));

	// MOUSE HIGHLIGHT
	const auto highlightPosition = position() + NAS2D::Vector{0, static_cast<int>(mHighlightIndex) * mItemSize.y - mScrollOffsetInPixels};
	renderer.drawBoxFilled(NAS2D::Rectangle<int>{highlightPosition, mItemSize}, NAS2D::Color{0, 185, 0, 50});

	drawItems();

	renderer.clipRectClear();
}
