// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ListBox.h"

#include "../../Constants.h"


/**
 * C'tor
 */
ListBox::ListBox()
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &ListBox::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().connect(this, &ListBox::onMouseMove);
	Utility<EventHandler>::get().mouseWheel().connect(this, &ListBox::onMouseWheel);
	
	init();
}


/**
 * D'tor
 */
ListBox::~ListBox()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &ListBox::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &ListBox::onMouseMove);
	Utility<EventHandler>::get().mouseWheel().disconnect(this, &ListBox::onMouseWheel);

	mSlider.change().disconnect(this, &ListBox::slideChanged);
}

/**
*
*/
void ListBox::init()
{
	mSlider.length(0);
	mSlider.thumbPosition(0);
	mSlider.change().connect(this, &ListBox::slideChanged);
	_updateItemDisplay();
}


void ListBox::onSizeChanged()
{
	clear();
	add(&mSlider, rect().width() - 14, 0);
	mSlider.displayPosition(false);
	mSlider.size(14, rect().height());
	_updateItemDisplay();
}


void ListBox::onFontChanged()
{
	mSlider.font(font());
	mLineHeight = (font().height() + 2);
	mLineCount = static_cast<int>(rect().height() / mLineHeight);
	_updateItemDisplay();
}


void ListBox::visibilityChanged(bool visible)
{
	_updateItemDisplay();
}


void ListBox::_updateItemDisplay()
{
	mItemWidth = rect().width();

	if ((mLineHeight * mItems.size()) > static_cast<size_t>(height()))
	{
		mLineCount = static_cast<int>(height() / mLineHeight);
		if (mLineCount < static_cast<int>(mItems.size()))
		{
			mSlider.length((mLineHeight * mItems.size()) - height());
			mCurrentOffset = mSlider.thumbPosition();
			mItemWidth = width() - mSlider.width();
			mSlider.visible(true);
		}
	}
	else
	{
		mCurrentOffset = 0;
		mSlider.length(0);
		mSlider.visible(false);
	}
}


/**
 * Gets whether the menu is empty or not.
 */
bool ListBox::empty() const
{
	return mItems.empty();
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
void ListBox::addItem(const std::string& item)
{
	mItems.push_back(item);
	sort();
	_updateItemDisplay();
}


/**
 * Removes a named item from the Menu.
 *
 * \todo	Complete this function.
 *
 * \note	Safe to call if Menu is empty.
 */
void ListBox::removeItem(const std::string& item)
{
	// Ignore if menu is empty
	if (empty()) { return; }

	StringList::iterator it = mItems.begin();

	while (it != mItems.end())
	{
		if (toLowercase((*it)) == toLowercase(item))
		{
			mItems.erase(it);
			mCurrentSelection = constants::NO_SELECTION;
			sort();
			_updateItemDisplay();
			return;
		}

		++it;
	}
}


bool ListBox::itemExists(const std::string& item)
{
	// Ignore if menu is empty
	if (empty()) { return false; }

	for(size_t i = 0; i < mItems.size(); i++)
	{
		if (toLowercase(mItems[i]) == toLowercase(item)) { return true; }
	}

	return false;
}


/**
 * Drops all items from the list.
 */
void ListBox::dropAllItems()
{
	mItems.clear();
	mCurrentSelection = 0;
	_updateItemDisplay();
}


void ListBox::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	// Ignore if menu is empty or invisible
	if (empty() || !visible()) { return; }

	if (!isPointInRect(Point_2d(x, y), rect()) || mCurrentHighlight == constants::NO_SELECTION)
	{
		return;
	}

	if (mSlider.visible() && isPointInRect(Point_2d(x, y), mSlider.rect()))
	{
		return;		// if the mouse is on the slider then the slider should handle that
	}

	if (mCurrentHighlight < 0 || static_cast<size_t>(mCurrentHighlight) >= mItems.size())
	{
		return;
	}

	currentSelection(mCurrentHighlight);
}


void ListBox::onMouseMove(int x, int y, int relX, int relY)
{
	// Ignore if menu is empty or invisible
	if (empty() || !visible()) { return; }

	// Ignore mouse motion events if the pointer isn't within the menu rect.
	if (!isPointInRect(Point_2d(x, y), rect()))
	{
		mCurrentHighlight = constants::NO_SELECTION;
		return;
	}

	// if the mouse is on the slider then the slider should handle that
	if (mSlider.visible() && isPointInRect(Point_2d(x, y), mSlider.rect()))
	{
		mCurrentHighlight = constants::NO_SELECTION;
		return;
	}
	
	mCurrentHighlight = (y - (int)rect().y() + mCurrentOffset) / (font().height() + 2);

	if (mCurrentHighlight < 0)
	{
		mCurrentHighlight = constants::NO_SELECTION;
	}

	if (static_cast<size_t>(mCurrentHighlight) >= mItems.size())
	{
		mCurrentHighlight = constants::NO_SELECTION;
	}
}


/**
 * \todo	Make the scroll amount configurable.
 */
void ListBox::onMouseWheel(int x, int y)
{
	if (empty() || !visible()) { return; }

	mSlider.changeThumbPosition((y < 0 ? 16.0 : -16.0));
}


void ListBox::update()
{
	// Ignore if menu is empty or invisible
	if (!visible()) { return; }

	Renderer& r = Utility<Renderer>::get();

	if (empty())
	{
		r.drawBoxFilled(rect(), 0, 0, 0);
		hasFocus() ? r.drawBox(rect(), 0, 185, 0) : r.drawBox(rect(), 75, 75, 75);
		return;
	}

	r.clipRect(rect().x() - 1, rect().y(), rect().width() + 1, rect().height() + 1);

	// draw boundaries of the widget
	r.drawBox(rect().x(), rect().y(), mItemWidth, rect().height(), 0, 0, 0, 100);
	r.drawBoxFilled(rect().x(), rect().y(), mItemWidth, rect().height(), 0, 85, 0, 220);

	// Highlight currently selected file
	int itemY = rect().y() + (mCurrentSelection * mLineHeight) - mCurrentOffset;
	r.drawBoxFilled(rect().x(), itemY, mItemWidth, mLineHeight, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue(), 80);
	
	// Highlight On mouse Over

	if (mCurrentHighlight != constants::NO_SELECTION)
	{
		itemY = rect().y() + (mCurrentHighlight * mLineHeight) - mCurrentOffset;
		r.drawBox(rect().x(), itemY, mItemWidth, mLineHeight, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue());
	}
	
	// display actuals values that are meant to be
	for(int i = 0; i < (int)mItems.size(); i++)
	{
		itemY = rect().y() + (i * mLineHeight) - mCurrentOffset;
		if (i == mCurrentHighlight)
		{
			r.drawTextShadow(font(), mItems[i], rect().x(), itemY, 1, mHighlightText.red(), mHighlightText.green(), mHighlightText.blue(), 0, 0, 0);
		}
		else
		{
			r.drawTextShadow(font(), mItems[i], rect().x(), itemY, 1, mText.red(), mText.green(), mText.blue(), 0, 0, 0);
		}
	}

	mSlider.update();		// Shouldn't need this since it's in a UIContainer. Noticing that Slider
							// doesn't play nice with the UIContainer.
	r.clipRectClear();
}


void ListBox::slideChanged(double _position)
{
	_updateItemDisplay();
	int pos = static_cast<int>(_position);
	if (static_cast<float>(pos) != _position)
	{
		mSlider.thumbPosition(static_cast<double>(pos));
	}
}
