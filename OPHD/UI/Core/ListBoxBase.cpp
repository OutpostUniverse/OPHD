#include "ListBoxBase.h"

#include "../../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/MathUtils.h>

#include <algorithm>
#include <cmath>


using namespace NAS2D;


ListBoxBase::ListBoxBase()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseWheel().connect(this, &ListBoxBase::onMouseWheel);
	eventHandler.mouseButtonDown().connect(this, &ListBoxBase::onMouseDown);
	eventHandler.mouseMotion().connect(this, &ListBoxBase::onMouseMove);

	mSlider.displayPosition(false);
	mSlider.length(0);
	mSlider.thumbPosition(0);
	mSlider.change().connect(this, &ListBoxBase::onSlideChange);

	updateScrollLayout();
}


ListBoxBase::~ListBoxBase()
{
	mSlider.change().disconnect(this, &ListBoxBase::onSlideChange);

	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseWheel().disconnect(this, &ListBoxBase::onMouseWheel);
	eventHandler.mouseButtonDown().disconnect(this, &ListBoxBase::onMouseDown);
	eventHandler.mouseMotion().disconnect(this, &ListBoxBase::onMouseMove);

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
	mItemWidth = mRect.width;

	if ((mItemHeight * static_cast<int>(mItems.size())) > mRect.height)
	{
		mSlider.position({rect().x + mRect.width - 14, mRect.y});
		mSlider.size({14, mRect.height});
		mSlider.length(static_cast<float>(mItemHeight * static_cast<int>(mItems.size()) - mRect.height));
		mScrollOffsetInPixels = static_cast<unsigned int>(mSlider.thumbPosition());
		mItemWidth -= static_cast<unsigned int>(mSlider.size().x);
		mSlider.visible(true);
	}
	else
	{
		mScrollOffsetInPixels = 0;
		mSlider.length(0);
		mSlider.visible(false);
	}
}


/**
 * Resized event handler.
 */
void ListBoxBase::onResize()
{
	updateScrollLayout();
}


/**
 * Mouse Down event handler.
 */
void ListBoxBase::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	const auto point = NAS2D::Point{x, y};

	if (!enabled() || !visible()) { return; }

	if (isEmpty() || button == EventHandler::MouseButton::Middle) { return; }

	if (button == EventHandler::MouseButton::Right && mRect.contains(point))
	{
		setSelection(constants::NoSelection);
		return;
	}

	// A few basic checks
	if (!rect().contains(point) || mHighlightIndex == constants::NoSelection) { return; }
	if (mSlider.visible() && mSlider.rect().contains(point)) { return; }
	if (mHighlightIndex >= mItems.size()) { return; }

	setSelection(mHighlightIndex);
}


/**
 * Mouse Motion event handler.
 */
void ListBoxBase::onMouseMove(int x, int y, int /*relX*/, int /*relY*/)
{
	if (!visible() || isEmpty()) { return; }

	const auto mousePosition = NAS2D::Point{x, y};
	mHasFocus = rect().contains(mousePosition);

	// Ignore mouse motion events if the pointer isn't within the menu rect.
	if (!mHasFocus)
	{
		mHighlightIndex = constants::NoSelection;
		return;
	}

	// if the mouse is on the slider then the slider should handle that
	if (mSlider.visible() && mSlider.rect().contains(mousePosition))
	{
		mHighlightIndex = constants::NoSelection;
		return;
	}

	mHighlightIndex = (static_cast<unsigned int>(y - positionY()) + mScrollOffsetInPixels) / static_cast<unsigned int>(mItemHeight);

	if (mHighlightIndex >= mItems.size())
	{
		mHighlightIndex = constants::NoSelection;
	}
}


/**
 * Mouse Wheel event handler.
 * 
 * \todo	Make the scroll step configurable. Legacy from the ListBox.
 */
void ListBoxBase::onMouseWheel(int /*x*/, int y)
{
	if (!enabled() || !visible()) { return; }
	if (!mHasFocus) { return; }

	float change = static_cast<float>(mItemHeight);

	mSlider.changeThumbPosition((y < 0 ? change : -change));
}


/**
 * Slider changed event handler.
 */
void ListBoxBase::onSlideChange(float newPosition)
{
	updateScrollLayout();
	auto pos = std::floor(newPosition);
	if (pos != newPosition)
	{
		mSlider.thumbPosition(pos);
	}
}


/**
 * Clears all items from the list.
 */
void ListBoxBase::clear()
{
	for (auto item : mItems) { delete item; }
	mItems.clear();
	mSelectedIndex = constants::NoSelection;
	mHighlightIndex = constants::NoSelection;
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
	return mSelectedIndex != constants::NoSelection;
}


const ListBoxBase::ListBoxItem& ListBoxBase::selected() const
{
	if (mSelectedIndex == constants::NoSelection)
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
	mSelectedIndex = (selection < mItems.size()) ? selection : constants::NoSelection;
	mSelectionChanged();
}


/**
 * Clears the current selection.
 */
void ListBoxBase::clearSelected()
{
	mSelectedIndex = constants::NoSelection;
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

	auto& renderer = Utility<Renderer>::get();

	// CONTROL EXTENTS
	const auto backgroundRect = NAS2D::Rectangle{mRect.x, mRect.y, mItemWidth, mRect.height};
	renderer.drawBoxFilled(backgroundRect, NAS2D::Color::Black);
	renderer.drawBox(backgroundRect, (hasFocus() ? NAS2D::Color{0, 185, 0} : NAS2D::Color{75, 75, 75}));

	renderer.clipRect(mRect);

	// MOUSE HIGHLIGHT
	int highlight_y = positionY() + (static_cast<int>(mHighlightIndex) * mItemHeight) - static_cast<int>(mScrollOffsetInPixels);
	renderer.drawBoxFilled(NAS2D::Rectangle{positionX(), highlight_y, mItemWidth, mItemHeight}, NAS2D::Color{0, 185, 0, 50});

	mSlider.update();

	renderer.clipRectClear();
}
