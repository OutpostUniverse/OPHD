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
	_updateItemDisplay();
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
	_updateItemDisplay();
}


void ListBox::visibilityChanged(bool /*visible*/)
{
	_updateItemDisplay();
}


void ListBox::_updateItemDisplay()
{
	// Account for border around control
	mScrollArea = mRect.inset(1);

	if ((mLineHeight * mItems.size()) > static_cast<std::size_t>(mRect.height))
	{
		const auto lineCount = static_cast<unsigned int>(mRect.height) / mLineHeight;
		if (lineCount < mItems.size())
		{
			mSlider.position({rect().x + mRect.width - 14, mRect.y});
			mSlider.size({14, mRect.height});
			mSlider.length(static_cast<float>(static_cast<int>(mLineHeight * mItems.size()) - mRect.height));
			mScrollOffsetInPixels = static_cast<std::size_t>(mSlider.thumbPosition());
			mScrollArea.width -= mSlider.size().x; // Remove scroll bar from scroll area
			mSlider.visible(true);
		}
	}
	else
	{
		mScrollOffsetInPixels = 0;
		mSlider.length(0);
		mSlider.visible(false);
	}
}


/**
 * Adds an item to the Menu.
 *
 * \param	item	Item to add.
 *
 * \warning	Menu::font(Font& font) must have been called with a valid Font
 *			before this function can be safely called.
 *
 * \todo	Make this function safe to call regardless of whether a font
 *			has been defined or not.
 */
void ListBox::addItem(const std::string& item, int tag)
{
	mItems.push_back(ListBoxItem{item, tag});
	_updateItemDisplay();
}


/**
 * Removes a named item from the Menu.
 */
void ListBox::removeItem(const std::string& item)
{
	auto it = std::find(mItems.begin(), mItems.end(), item);
	if (it != mItems.end())
	{
		mItems.erase(it);
		mSelectedIndex = constants::NO_SELECTION;
		_updateItemDisplay();
	}
}


bool ListBox::itemExists(const std::string& item)
{
	return std::find(mItems.begin(), mItems.end(), item) != mItems.end();
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
	_updateItemDisplay();
}


void ListBox::sort()
{
	std::sort(mItems.begin(), mItems.end());
}


void ListBox::onMouseDown(EventHandler::MouseButton /*button*/, int x, int y)
{
	if (!visible() || mHighlightIndex == constants::NO_SELECTION || mHighlightIndex >= mItems.size()) { return; }

	const auto point = NAS2D::Point{x, y};

	if (!rect().contains(point))
	{
		return;
	}

	if (mSlider.visible() && mSlider.rect().contains(point))
	{
		return; // if the mouse is on the slider then the slider should handle that
	}

	setSelected(mHighlightIndex);
}


void ListBox::onMouseMove(int x, int y, int /*relX*/, int /*relY*/)
{
	// Ignore if menu is empty or invisible
	if (isEmpty() || !visible()) { return; }

	const auto point = NAS2D::Point{x, y};

	if (!mScrollArea.contains(point))
	{
		mHighlightIndex = constants::NO_SELECTION;
		return;
	}

	mHighlightIndex = (static_cast<std::size_t>(y - mRect.y) + mScrollOffsetInPixels) / static_cast<std::size_t>(mLineHeight);
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
	auto textPosition = mScrollArea.startPoint();
	textPosition += {constants::MARGIN_TIGHT, -static_cast<int>(mScrollOffsetInPixels)};
	for(std::size_t i = 0; i < mItems.size(); i++)
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
	_updateItemDisplay();
	// Intentional truncation of fractional value
	const auto pos = std::floor(newPosition);
	if (pos != newPosition)
	{
		mSlider.thumbPosition(pos);
	}
}
