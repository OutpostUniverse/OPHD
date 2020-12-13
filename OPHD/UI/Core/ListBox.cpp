#include "ListBox.h"

#include "../../Constants.h"
#include "../../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/MathUtils.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Renderer/Point.h>

#include <algorithm>
#include <cmath>


using namespace NAS2D;


ListBox::ListBox() :
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL)},
	mLineHeight{static_cast<unsigned int>(mFont.height() + constants::MARGIN_TIGHT)}
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &ListBox::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().connect(this, &ListBox::onMouseMove);
	Utility<EventHandler>::get().mouseWheel().connect(this, &ListBox::onMouseWheel);

	mSlider.displayPosition(false);
	mSlider.length(0);
	mSlider.thumbPosition(0);
	mSlider.change().connect(this, &ListBox::slideChanged);
	updateScrollLayout();
}


ListBox::~ListBox()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &ListBox::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &ListBox::onMouseMove);
	Utility<EventHandler>::get().mouseWheel().disconnect(this, &ListBox::onMouseWheel);

	mSlider.change().disconnect(this, &ListBox::slideChanged);
}


/**
 * Gets whether the menu is empty or not.
 */
bool ListBox::isEmpty() const
{
	return mItems.empty();
}


void ListBox::onSizeChanged()
{
	updateScrollLayout();
}


void ListBox::visibilityChanged(bool /*visible*/)
{
	updateScrollLayout();
}


void ListBox::updateScrollLayout()
{
	// Account for border around control
	mScrollArea = mRect.inset(1);

	const auto neededDisplaySize = mLineHeight * mItems.size();
	if (neededDisplaySize > static_cast<std::size_t>(mRect.height))
	{
		mSlider.position({rect().x + mRect.width - 14, mRect.y});
		mSlider.size({14, mRect.height});
		mSlider.length(static_cast<float>(static_cast<int>(neededDisplaySize) - mRect.height));
		mScrollOffsetInPixels = static_cast<std::size_t>(mSlider.thumbPosition());
		mScrollArea.width -= mSlider.size().x; // Remove scroll bar from scroll area
		mSlider.visible(true);
	}
	else
	{
		mScrollOffsetInPixels = 0;
		mSlider.length(0);
		mSlider.visible(false);
	}
}


bool ListBox::isItemSelected() const
{
	return mSelectedIndex != constants::NO_SELECTION;
}


const ListBox::ListBoxItem& ListBox::selected() const
{
	if (mSelectedIndex == constants::NO_SELECTION)
	{
		throw std::runtime_error("ListBox has no selected item");
	}

	return mItems[mSelectedIndex];
}


/**
 * Drops all items from the list.
 */
void ListBox::clear()
{
	mItems.clear();
	mSelectedIndex = constants::NO_SELECTION;
	mHighlightIndex = constants::NO_SELECTION;
	updateScrollLayout();
}


void ListBox::sort()
{
	std::sort(mItems.begin(), mItems.end());
}


void ListBox::onMouseDown(EventHandler::MouseButton /*button*/, int x, int y)
{
	if (!visible() || mHighlightIndex == constants::NO_SELECTION || mHighlightIndex >= mItems.size() || !mScrollArea.contains({x, y}))
	{
		return;
	}

	setSelected(mHighlightIndex);
}


void ListBox::onMouseMove(int x, int y, int /*relX*/, int /*relY*/)
{
	if (!visible() || !mScrollArea.contains({x, y}))
	{
		mHighlightIndex = constants::NO_SELECTION;
		return;
	}

	mHighlightIndex = (static_cast<std::size_t>(y - mScrollArea.y) + mScrollOffsetInPixels) / static_cast<std::size_t>(mLineHeight);
	if (mHighlightIndex >= mItems.size())
	{
		mHighlightIndex = constants::NO_SELECTION;
	}
}


/**
 * \todo	Make the scroll amount configurable.
 */
void ListBox::onMouseWheel(int /*x*/, int y)
{
	if (isEmpty() || !visible()) { return; }

	mSlider.changeThumbPosition((y < 0 ? 16.0f : -16.0f));
}


void ListBox::update()
{
	// Ignore if menu is empty or invisible
	if (!visible()) { return; }

	auto& renderer = Utility<Renderer>::get();

	const auto borderColor = hasFocus() ? mBorderColorActive : mBorderColorNormal;
	renderer.drawBox(mRect, borderColor);
	renderer.drawBoxFilled(mScrollArea, mBackgroundColorNormal);

	renderer.clipRect(mRect);

	// Highlight currently selected item
	auto itemBounds = mScrollArea;
	itemBounds.height = static_cast<int>(mLineHeight);
	itemBounds.y += static_cast<int>((mSelectedIndex * mLineHeight) - mScrollOffsetInPixels);
	renderer.drawBoxFilled(itemBounds, mBackgroundColorSelected);

	// Highlight On mouse Over
	if (mHighlightIndex != constants::NO_SELECTION)
	{
		auto highlightBounds = mScrollArea;
		highlightBounds.height = static_cast<int>(mLineHeight);
		highlightBounds.y += static_cast<int>((mHighlightIndex * mLineHeight) - mScrollOffsetInPixels);
		renderer.drawBox(highlightBounds, mBackgroundColorMouseHover);
	}

	// display actuals values that are meant to be
	const auto firstVisibleIndex = mScrollOffsetInPixels / mLineHeight;
	const auto lastVisibleIndex = (mScrollOffsetInPixels + mScrollArea.height + (mLineHeight - 1)) / mLineHeight;
	const auto endVisibleIndex = std::min(lastVisibleIndex, mItems.size());
	auto textPosition = mScrollArea.startPoint();
	textPosition += {constants::MARGIN_TIGHT, -static_cast<int>(mScrollOffsetInPixels % mLineHeight)};
	for(std::size_t i = firstVisibleIndex; i < endVisibleIndex; i++)
	{
		const auto textColor = (i == mHighlightIndex) ? mTextColorMouseHover : mTextColorNormal;
		renderer.drawTextShadow(mFont, mItems[i].text, textPosition, {1, 1}, textColor, NAS2D::Color::Black);
		textPosition.y += mLineHeight;
	}

	renderer.clipRectClear();

	mSlider.update();
}


void ListBox::slideChanged(float newPosition)
{
	updateScrollLayout();
	// Intentional truncation of fractional value
	const auto pos = std::floor(newPosition);
	if (pos != newPosition)
	{
		mSlider.thumbPosition(pos);
	}
}
