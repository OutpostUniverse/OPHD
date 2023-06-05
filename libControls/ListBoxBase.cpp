#include "ListBoxBase.h"


#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/MathUtils.h>

#include <algorithm>


ListBoxBase::ListBoxBase()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().connect({this, &ListBoxBase::onMouseWheel});
	eventHandler.mouseButtonDown().connect({this, &ListBoxBase::onMouseDown});
	eventHandler.mouseMotion().connect({this, &ListBoxBase::onMouseMove});

	mScrollBar.max(0);
	mScrollBar.value(0);
	mScrollBar.change().connect({this, &ListBoxBase::onSlideChange});

	updateScrollLayout();
}


ListBoxBase::~ListBoxBase()
{
	mScrollBar.change().disconnect({this, &ListBoxBase::onSlideChange});

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().disconnect({this, &ListBoxBase::onMouseWheel});
	eventHandler.mouseButtonDown().disconnect({this, &ListBoxBase::onMouseDown});
	eventHandler.mouseMotion().disconnect({this, &ListBoxBase::onMouseMove});

	for (auto item : mItems) { delete item; }
}


/**
 * True if no items are in the list.
 */
bool ListBoxBase::isEmpty() const
{
	return mItems.empty();
}


/**
 * Number of items in the ListBoxBase.
 */
std::size_t ListBoxBase::count() const
{
	return mItems.size();
}


void ListBoxBase::onVisibilityChange(bool)
{
	updateScrollLayout();
}


/**
 * Updates values required for properly displaying list items.
 */
void ListBoxBase::updateScrollLayout()
{
	mItemWidth = mRect.size.x;

	if ((mItemHeight * static_cast<int>(mItems.size())) > mRect.size.y)
	{
		mScrollBar.position({rect().position.x + mRect.size.x - 14, mRect.position.y});
		mScrollBar.size({14, mRect.size.y});
		mScrollBar.max(static_cast<ScrollBar::ValueType>(mItemHeight * static_cast<int>(mItems.size()) - mRect.size.y));
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


/**
 * Resized event handler.
 */
void ListBoxBase::onResize()
{
	updateScrollLayout();
}


void ListBoxBase::onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	if (isEmpty() || button == NAS2D::EventHandler::MouseButton::Middle) { return; }

	if (button == NAS2D::EventHandler::MouseButton::Right && mRect.contains(position))
	{
		setSelection(NoSelection);
		return;
	}

	// A few basic checks
	if (!rect().contains(position) || mHighlightIndex == NoSelection) { return; }
	if (mScrollBar.visible() && mScrollBar.rect().contains(position)) { return; }
	if (mHighlightIndex >= mItems.size()) { return; }

	setSelection(mHighlightIndex);
}


void ListBoxBase::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (!visible() || isEmpty()) { return; }

	mHasFocus = rect().contains(position);

	// Ignore mouse motion events if the pointer isn't within the menu rect.
	if (!mHasFocus)
	{
		mHighlightIndex = NoSelection;
		return;
	}

	// if the mouse is on the scroll bar then the scroll bar should handle that
	if (mScrollBar.visible() && mScrollBar.rect().contains(position))
	{
		mHighlightIndex = NoSelection;
		return;
	}

	mHighlightIndex = (static_cast<unsigned int>(position.y - positionY()) + mScrollOffsetInPixels) / static_cast<unsigned int>(mItemHeight);

	if (mHighlightIndex >= mItems.size())
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


/**
 * ScrollBar changed event handler.
 */
void ListBoxBase::onSlideChange(ScrollBar::ValueType /*newPosition*/)
{
	updateScrollLayout();
}


/**
 * Clears all items from the list.
 */
void ListBoxBase::clear()
{
	for (auto item : mItems) { delete item; }
	mItems.clear();
	mSelectedIndex = NoSelection;
	mHighlightIndex = NoSelection;
	updateScrollLayout();
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


const ListBoxBase::ListBoxItem& ListBoxBase::selected() const
{
	if (mSelectedIndex == NoSelection)
	{
		throw std::runtime_error("ListBox has no selected item");
	}

	return *mItems[mSelectedIndex];
}


/**
 * Sets the current selection index.
 * 
 * \note	Out of range selection indicies will set the ListBoxBase to no selection.
 */
void ListBoxBase::setSelection(std::size_t selection)
{
	mSelectedIndex = (selection < mItems.size()) ? selection : NoSelection;
	mSelectionChanged();
}


/**
 * Clears the current selection.
 */
void ListBoxBase::clearSelected()
{
	mSelectedIndex = NoSelection;
}


/**
 * Sets item height.
 * 
 * \note	Internal function for specialized types.
 */
void ListBoxBase::item_height(int h)
{
	mItemHeight = h;
}


/**
 * Draws the ListBox
 */
void ListBoxBase::update()
{
	if (!visible()) { return; }
	draw();
	mScrollBar.update();
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
	int highlight_y = positionY() + (static_cast<int>(mHighlightIndex) * mItemHeight) - static_cast<int>(mScrollOffsetInPixels);
	renderer.drawBoxFilled(NAS2D::Rectangle<int>{{positionX(), highlight_y}, {mItemWidth, mItemHeight}}, NAS2D::Color{0, 185, 0, 50});

	renderer.clipRectClear();
}
