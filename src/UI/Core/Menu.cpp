#include "Menu.h"

#include "../../Constants.h"

#include <iostream>

using namespace std;


/**
 * C'tor
 */
Menu::Menu():	mCurrentHighlight(constants::NO_SELECTION),
				mCurrentSelection(0),
				mText(COLOR_WHITE),
				mHighlightBg(COLOR_GREEN),
				mHighlightText(COLOR_WHITE),
				mSorted(false)
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &Menu::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().connect(this, &Menu::onMouseMove);
}


/**
 * D'tor
 */
Menu::~Menu()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &Menu::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &Menu::onMouseMove);
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

	if(font().width(item) > _rect().width())
		_rect().width() = font().width(item) + 2;

	_rect().height() = mItems.size() * (font().height() + 2);

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
			_rect().height() = mItems.size() * (font().height() + 2);
			mCurrentSelection = constants::NO_SELECTION;
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

	_rect().x(x);
	_rect().y(y);
}


void Menu::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	// Ignore if menu is empty or invisible
	if(empty() || !visible())
		return;

	if(!isPointInRect(Point_2d(x, y), _rect()) || mCurrentHighlight == constants::NO_SELECTION)
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
	if(!isPointInRect(Point_2d(x, y), _rect()))
	{
		mCurrentHighlight = constants::NO_SELECTION;
		return;
	}

	mCurrentHighlight = ((y - (int)_rect().y()) / (font().height() + 2)) % ((int)_rect().height() / (font().height() + 2));
}


void Menu::update()
{
	// Ignore if menu is empty or invisible
	if(empty() || !visible())
		return;

	Renderer& r = Utility<Renderer>::get();

	int line_height = (font().height() + 2);

	r.drawBox(_rect(), 0, 0, 0, 100);
	r.drawBoxFilled(_rect(), 225, 225, 0, 85);

	r.drawBoxFilled(rect().x(), rect().y() + (mCurrentSelection * line_height), rect().width(), line_height, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue(), 80);

	if(mCurrentHighlight != constants::NO_SELECTION)
		r.drawBox(_rect().x(), _rect().y() + (mCurrentHighlight * line_height), _rect().width(), line_height, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue());

	for(size_t i = 0; i < mItems.size(); i++)
	{
		if(i == mCurrentHighlight)
			r.drawTextShadow(font(), mItems[i], _rect().x(), _rect().y() + (i * line_height), 1, mHighlightText.red(), mHighlightText.green(), mHighlightText.blue(), 0, 0, 0);
		else
			r.drawTextShadow(font(), mItems[i], _rect().x(), _rect().y() + (i * line_height), 1, mText.red(), mText.green(), mText.blue(), 0, 0, 0);
	}
}
