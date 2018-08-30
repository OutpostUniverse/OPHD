// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Menu.h"

#include "../../Constants.h"

#include <algorithm>
#include <iostream>


/**
 * C'tor
 */
Menu::Menu()
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


int Menu::currentSelection() const
{
	return mCurrentSelection;
}


void Menu::currentSelection(int selection)
{
	mCurrentSelection = selection; mSelectionChanged();
}


const std::string& Menu::selectionText() const
{
	return mItems[mCurrentSelection];
}


void Menu::textColor(const Color_4ub& color)
{
	mText = color;
}


void Menu::selectColor(const Color_4ub& color)
{
	mHighlightBg = color;
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
void Menu::addItem(const std::string& item)
{
	mItems.push_back(item);

	if (font().width(item) > _rect().width())
	{
		_rect().width() = font().width(item) + 2;
	}

	_rect().height() = static_cast<float>(mItems.size() * (font().height() + 2));
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
	if (empty()) { return; }

	StringList::iterator it = mItems.begin();

	while(it != mItems.end())
	{
		if(toLowercase((*it)) == toLowercase(item))
		{
			mItems.erase(it);
			_rect().height() = static_cast<float>(mItems.size() * (font().height() + 2));
			mCurrentSelection = constants::NO_SELECTION;
			return;
		}

		++it;
	}

	sort();
}


void Menu::sort()
{
	std::sort(mItems.begin(), mItems.end());
}


bool Menu::itemExists(const std::string& item)
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
void Menu::clear()
{
	mItems.clear();
	mCurrentSelection = constants::NO_SELECTION;
}


void Menu::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (empty() || !visible()) { return; }

	if (!isPointInRect(Point_2d(x, y), _rect()) || mCurrentHighlight == constants::NO_SELECTION) { return; }

	currentSelection(mCurrentHighlight);
}


void Menu::onMouseMove(int x, int y, int relX, int relY)
{
	if (empty() || !visible()) { return; }

	if(!isPointInRect(Point_2d(x, y), _rect()))
	{
		mCurrentHighlight = constants::NO_SELECTION;
		return;
	}

	mCurrentHighlight = ((y - (int)_rect().y()) / (font().height() + 2)) % ((int)_rect().height() / (font().height() + 2));
}


void Menu::update()
{
	if (empty() || !visible()) { return; }

	Renderer& r = Utility<Renderer>::get();

	int line_height = (font().height() + 2);

	r.drawBox(_rect(), 0, 0, 0, 100);
	r.drawBoxFilled(_rect(), 225, 225, 0, 85);

	if (mCurrentSelection != constants::NO_SELECTION)
	{
		r.drawBoxFilled(rect().x(), rect().y() + (mCurrentSelection * line_height), rect().width(), line_height, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue(), 80);
	}

	if (mCurrentHighlight != constants::NO_SELECTION)
	{
		r.drawBox(_rect().x(), _rect().y() + (mCurrentHighlight * line_height), _rect().width(), line_height, mHighlightBg.red(), mHighlightBg.green(), mHighlightBg.blue());
	}

	for(int i = 0; (unsigned)i < mItems.size(); i++)
	{
		if (i == mCurrentHighlight)
		{
			r.drawTextShadow(font(), mItems[i], _rect().x(), _rect().y() + (i * line_height), 1, mHighlightText.red(), mHighlightText.green(), mHighlightText.blue(), 0, 0, 0);
		}
		else
		{
			r.drawTextShadow(font(), mItems[i], _rect().x(), _rect().y() + (i * line_height), 1, mText.red(), mText.green(), mText.blue(), 0, 0, 0);
		}
	}
}
