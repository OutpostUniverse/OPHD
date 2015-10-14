#include "Menu.h"

#include <iostream>

using namespace std;

const int NO_SELECTION = -1;

/**
 * C'tor
 */
Menu::Menu():	mCurrentHighlight(-1),
				mCurrentSelection(0),
				mText(COLOR_WHITE),
				mHighlightBg(COLOR_GREEN),
				mHighlightText(COLOR_WHITE),
				mSorted(false)
{
	Utility<EventHandler>::get().mouseButtonDown().Connect(this, &Menu::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().Connect(this, &Menu::onMouseMove);
}


/**
 * D'tor
 */
Menu::~Menu()
{
	Utility<EventHandler>::get().mouseButtonDown().Disconnect(this, &Menu::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().Disconnect(this, &Menu::onMouseMove);
}

/**
 * Gets whether the menu is empty or not.
 */
bool Menu::empty() const
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
void Menu::addItem(const string& item)
{
	mItems.push_back(item);

	if(font().width(item) > mRect.w())
		mRect.w() = font().width(item) + 2;

	mRect.h() = mItems.size() * (font().height() + 2);

	sort();
}


/**
 * Removes a named item from the Menu.
 * 
 * \todo	Complete this function.
 * 
 * \note	Safe to call if Menu is empty.
 */
void Menu::removeItem(const std::string& item)
{
	// Ignore if menu is empty
	if(empty())
		return;

	StringList::iterator it = mItems.begin();

	while(it != mItems.end())
	{
		if(toLowercase((*it)) == toLowercase(item))
		{
			mItems.erase(it);
			mRect.h() = mItems.size() * (font().height() + 2);
			mCurrentSelection = NO_SELECTION;
			return;
		}
		
		++it;
	}

	sort();
}


bool Menu::itemExists(const std::string& item)
{
	// Ignore if menu is empty
	if(empty())
		return false;

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
void Menu::dropAllItems()
{
	mItems.clear();
	mCurrentSelection = 0;
}


/**
 * Sets the position of the menu.
 * 
 * \note	The Height of the menu is determined by the number of items
 *			items and not by the height of the rectangle.
 */
void Menu::position(int x, int y)
{

	mRect.x(x);
	mRect.y(y);
}


void Menu::onMouseDown(MouseButton button, int x, int y)
{
	// Ignore if menu is empty or invisible
	if(empty() || !visible())
		return;

	if(!isPointInRect(Point_2d(x, y), mRect) || mCurrentHighlight == NO_SELECTION)
		return;

	currentSelection(mCurrentHighlight);
	//mCurrentSelection = mCurrentHighlight;
}


void Menu::onMouseMove(int x, int y, int relX, int relY)
{
	// Ignore if menu is empty or invisible
	if(empty() || !visible())
		return;

	// Ignore mouse motion events if the pointer isn't within the menu rect.
	if(!isPointInRect(Point_2d(x, y), mRect))
	{
		mCurrentHighlight = NO_SELECTION;
		return;
	}

	mCurrentHighlight = ((y - mRect.y()) / (font().height() + 2)) % (mRect.h() / (font().height() + 2));
}


void Menu::update()
{
	// Ignore if menu is empty or invisible
	if(empty() || !visible())
		return;

	Renderer& r = Utility<Renderer>::get();

	int line_height = (font().height() + 2);

	r.drawBox(mRect, 0, 0, 0, 100);
	r.drawBoxFilled(mRect, 225, 225, 0, 85);

	//r.drawBoxFilled(mRect.x(), mRect.y() + (mCurrentSelection * line_height), mRect.w(), line_height, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue(), 80);

	if(mCurrentHighlight != NO_SELECTION)
		r.drawBox(mRect.x(), mRect.y() + (mCurrentHighlight * line_height), mRect.w(), line_height, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue());

	for(size_t i = 0; i < mItems.size(); i++)
	{
		if(i == mCurrentHighlight)
			r.drawTextShadow(font(), mItems[i], mRect.x(), mRect.y() + (i * line_height), 1, mHighlightText.red(), mHighlightText.green(), mHighlightText.blue(), 0, 0, 0);
		else
			r.drawTextShadow(font(), mItems[i], mRect.x(), mRect.y() + (i * line_height), 1, mText.red(), mText.green(), mText.blue(), 0, 0, 0);
	}
}
