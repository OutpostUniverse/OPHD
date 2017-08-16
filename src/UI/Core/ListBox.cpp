#include "ListBox.h"

#include "../../Constants.h"

using namespace std;


/**
 * C'tor
 */
ListBox::ListBox():	mCurrentHighlight(constants::NO_SELECTION),
					mCurrentSelection(0),
					mCurrentOffset(0),
					mText(COLOR_WHITE),
					mHighlightBg(COLOR_GREEN),
					mHighlightText(COLOR_WHITE),
					mSorted(false)
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &ListBox::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().connect(this, &ListBox::onMouseMove);
	
	init();
}


/**
 * D'tor
 */
ListBox::~ListBox()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &ListBox::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &ListBox::onMouseMove);

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
}

void ListBox::size(float w, float h)
{
	Control::size(w, h);
	deleteControl("mSlider");
	addControl("mSlider", &mSlider, rect().width() - 14, 0);
	mSlider.font(font());
	mSlider.displayPosition(false);
	mSlider.size(14, rect().height());
	_updateItemDisplay();
}


void ListBox::onFontChanged()
{
	mLineHeight = (font().height() + 2);
	_updateItemDisplay();
}


void ListBox::_updateItemDisplay()
{
	mItemWidth = rect().width();

	if ((mLineHeight * mItems.size()) > rect().height())
	{
		mLineCount = static_cast<int>(rect().height() / mLineHeight);
		if (mLineCount < static_cast<int>(mItems.size()))
		{
			mSlider.length((mLineHeight * mItems.size()) - rect().height());
			mSlider.visible(true);
			mCurrentOffset = mSlider.thumbPosition();
			mItemWidth = rect().width() - mSlider.rect().width();
		}
	}
	else
	{
		mCurrentOffset = 0;
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
void ListBox::addItem(const string& item)
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

	while(it != mItems.end())
	{
		if(toLowercase((*it)) == toLowercase(item))
		{
			mItems.erase(it);
			mCurrentSelection = constants::NO_SELECTION;
			return;
		}

		++it;
	}

	sort();
	_updateItemDisplay();
}


bool ListBox::itemExists(const std::string& item)
{
	// Ignore if menu is empty
	if (empty()) { return false; }

	for(size_t i = 0; i < mItems.size(); i++)
	{
		if(toLowercase(mItems[i]) == toLowercase(item))
			return true;
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


void ListBox::update()
{
	// Ignore if menu is empty or invisible
	if (empty() || !visible()) { return; }

	Renderer& r = Utility<Renderer>::get();
	r.clipRect(rect());

	// draw boundaries of the widget
	r.drawBox(rect().x(), rect().y(), mItemWidth, rect().height(), 0, 0, 0, 100);
	r.drawBoxFilled(rect().x(), rect().y(), mItemWidth, rect().height(), 225, 225, 0, 85);

	// Highlight currently selected file
	int itemY = rect().y() + (mCurrentSelection * mLineHeight) - mCurrentOffset;
	r.drawBoxFilled(rect().x(), itemY, mItemWidth, mLineHeight, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue(), 80);
	
	// Highlight On mouse Over

	if (mCurrentHighlight != constants::NO_SELECTION)
	{
		itemY = rect().y() + (mCurrentHighlight * mLineHeight) - mCurrentOffset;
		r.drawBox(rect().x(), itemY, mItemWidth, mLineHeight, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue());
	}
	
	
	//int iMin = 0;
	//int iMax = mItems.size();

	// display actuals values that are meant to be
	for(int i = 0; i < mItems.size(); i++)
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

	// draw the slider if needed
	mSlider.update();
	r.clipRectClear();
}


void ListBox::slideChanged(double _position)
{
	int pos = static_cast<int>(_position);
	if (static_cast<float>(pos) != _position)
	{
		mSlider.thumbPosition(static_cast<double>(pos));
	}
	_updateItemDisplay();
}
