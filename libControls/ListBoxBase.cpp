#include "ListBoxBase.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Point.h>

#include <algorithm>
#include <limits>


const std::size_t ListBoxBase::NoSelection{std::numeric_limits<std::size_t>::max()};


ListBoxBase::ListBoxBase(SelectionChangedDelegate selectionChangedHandler) :
	mSelectionChangedHandler{selectionChangedHandler}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().connect({this, &ListBoxBase::onMouseWheel});
	eventHandler.mouseButtonDown().connect({this, &ListBoxBase::onMouseDown});
	eventHandler.mouseMotion().connect({this, &ListBoxBase::onMouseMove});

	mScrollBar.max(0);
	mScrollBar.value(0);
	mScrollBar.change().connect({this, &ListBoxBase::onSlideChange});
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
	mItemWidth = mRect.size.x;

	if ((mItemHeight * static_cast<int>(count())) > mRect.size.y)
	{
		mScrollBar.position({area().position.x + mRect.size.x - 14, mRect.position.y});
		mScrollBar.size({14, mRect.size.y});
		mScrollBar.max(static_cast<ScrollBar::ValueType>(mItemHeight * static_cast<int>(count()) - mRect.size.y));
		mScrollOffsetInPixels = static_cast<unsigned int>(mScrollBar.value());
		mItemWidth -= mScrollBar.size().x;
		mScrollBar.visible(true);
	}
	else
	{
		mScrollOffsetInPixels = 0;
		mScrollBar.max(0);
		mScrollBar.visible(false);
	}
}


void ListBoxBase::onVisibilityChange(bool)
{
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
void ListBoxBase::onSlideChange(ScrollBar::ValueType /*newPosition*/)
{
	updateScrollLayout();
}


void ListBoxBase::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

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

	mHighlightIndex = (static_cast<unsigned int>(position.y - this->position().y) + mScrollOffsetInPixels) / static_cast<unsigned int>(mItemHeight);

	if (mHighlightIndex >= count())
	{
		mHighlightIndex = NoSelection;
	}
}


void ListBoxBase::onMouseWheel(NAS2D::Vector<int> scrollAmount)
{
	if (!enabled() || !visible()) { return; }
	if (!mHasFocus) { return; }

	auto change = static_cast<ScrollBar::ValueType>(mItemHeight);

	mScrollBar.changeValue((scrollAmount.y < 0 ? change : -change));
}


unsigned int ListBoxBase::itemWidth() const
{
	return static_cast<unsigned int>(mItemWidth);
}


unsigned int ListBoxBase::itemHeight() const
{
	return static_cast<unsigned int>(mItemHeight);
}


/**
 * Sets item height.
 *
 * \note	Internal function for specialized types.
 */
void ListBoxBase::itemHeight(int h)
{
	mItemHeight = h;
}


unsigned int ListBoxBase::drawOffset() const
{
	return mScrollOffsetInPixels;
}


NAS2D::Vector<int> ListBoxBase::itemDrawSize() const
{
	return NAS2D::Vector{itemWidth(), itemHeight()}.to<int>();
}


NAS2D::Point<int> ListBoxBase::itemDrawPosition(std::size_t index) const
{
	return position() + NAS2D::Vector{0, static_cast<int>(index * itemHeight() - drawOffset())};
}


NAS2D::Rectangle<int> ListBoxBase::itemDrawArea(std::size_t index) const
{
	return {itemDrawPosition(index), itemDrawSize()};
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

	draw();
	mScrollBar.update();

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.clipRect(mRect);

	for (std::size_t index = 0; index < count(); ++index)
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

	renderer.clipRectClear();
}


void ListBoxBase::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// CONTROL EXTENTS
	const auto backgroundRect = NAS2D::Rectangle<int>{{mRect.position.x, mRect.position.y}, {mItemWidth, mRect.size.y}};
	renderer.drawBoxFilled(backgroundRect, NAS2D::Color::Black);
	renderer.drawBox(backgroundRect, (hasFocus() ? NAS2D::Color{0, 185, 0} : NAS2D::Color{75, 75, 75}));

	renderer.clipRect(mRect);

	// MOUSE HIGHLIGHT
	const auto highlightPosition = position() + NAS2D::Vector{0, static_cast<int>(mHighlightIndex) * mItemHeight - static_cast<int>(mScrollOffsetInPixels)};
	renderer.drawBoxFilled(NAS2D::Rectangle<int>{highlightPosition, {mItemWidth, mItemHeight}}, NAS2D::Color{0, 185, 0, 50});

	renderer.clipRectClear();
}
