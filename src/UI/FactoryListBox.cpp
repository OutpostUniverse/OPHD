// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryListBox.h"

#include "../Constants.h"
#include "../FontManager.h"


using namespace NAS2D;


const int LIST_ITEM_HEIGHT = 58;
Image* STRUCTURE_ICONS = nullptr;

static Font* MAIN_FONT = nullptr;
static Font* MAIN_FONT_BOLD = nullptr;


static void drawItem(FactoryListBox::FactoryListBoxItem& item, int x, int y, int w, int offset, bool highlight)
{
	Renderer& r = Utility<Renderer>::get();

	if (highlight) { r.drawBoxFilled(x, y - offset, w, LIST_ITEM_HEIGHT, 0, 185, 0, 75); }

	r.drawBox(x + 2, y + 2, w - 4, LIST_ITEM_HEIGHT - 4, 0, 185, 0);

	r.drawSubImage(*STRUCTURE_ICONS, x + 8, y + 8, item.icon_slice.x(), item.icon_slice.y(), 46, 46);
	r.drawText(*MAIN_FONT_BOLD, item.factory->name(), x + 64, ((y + 29) - MAIN_FONT_BOLD->height() / 2) - offset, 0, 185, 0);
}




/**
 * C'tor
 */
FactoryListBox::FactoryListBox()
{
	_init();
}


/**
 * D'tor
 */
FactoryListBox::~FactoryListBox()
{
	mSlider.change().disconnect(this, &FactoryListBox::slideChanged);
	delete STRUCTURE_ICONS;
}


/**
 * Internal initializer function.
 */
void FactoryListBox::_init()
{
	MAIN_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 12);
	MAIN_FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, 12);

	mSlider.length(0);
	mSlider.thumbPosition(0);
	mSlider.change().connect(this, &FactoryListBox::slideChanged);

	STRUCTURE_ICONS = new Image("ui/structures.png");

	_update_item_display();
}


/**
 * Updates values required for properly displaying list items.
 */
void FactoryListBox::_update_item_display()
{
	mItemWidth = width();

	if ((LIST_ITEM_HEIGHT * mItems.size()) > static_cast<size_t>(height()))
	{
		mLineCount = static_cast<int>(height() / LIST_ITEM_HEIGHT);

		if (mLineCount < static_cast<int>(mItems.size()))
		{
			mSlider.length((LIST_ITEM_HEIGHT * mItems.size()) - height());
			mCurrentOffset = mSlider.thumbPosition();
			mItemWidth -= mSlider.width();
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
 * Internal function called during certain events
 * to hook or unhook input event handlers.
 */
void FactoryListBox::_hook_events(bool hook)
{
	if (hook)
	{
		Utility<EventHandler>::get().mouseButtonDown().connect(this, &FactoryListBox::onMouseDown);
		Utility<EventHandler>::get().mouseMotion().connect(this, &FactoryListBox::onMouseMove);
		Utility<EventHandler>::get().mouseWheel().connect(this, &FactoryListBox::onMouseWheel);
	}
	else
	{
		Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &FactoryListBox::onMouseDown);
		Utility<EventHandler>::get().mouseMotion().disconnect(this, &FactoryListBox::onMouseMove);
		Utility<EventHandler>::get().mouseWheel().disconnect(this, &FactoryListBox::onMouseWheel);
	}
}


/**
 * Adds a Factory to the FactoryListBox.
 */
void FactoryListBox::addItem(Factory* factory)
{
	// basic fuckup check.
	for (auto& item : mItems)
	{
		if (item.factory == factory)
		{
			std::cout << "FactoryListBox::addItem(): annoying bug, fix it." << std::endl;
			return;
		}
	}

	mItems.push_back(FactoryListBoxItem(factory));

	/// \fixme super sloppy, make this way more elegant.
	FactoryListBoxItem& item = mItems.back();
	if (factory->name() == constants::SURFACE_FACTORY) { item.icon_slice(0, 46); }
	else if (factory->name() == constants::UNDERGROUND_FACTORY) { item.icon_slice(138, 276); }
	else if (factory->name() == constants::SEED_FACTORY) { item.icon_slice(460, 368); }

	_update_item_display();
}


/**
 * Removes a Factory from the FactoryListBox.
 */
void FactoryListBox::removeItem(Factory* factory)
{
	// I know, not as neat as the range based loop or as elegant as a find() call
	// but necessary because I don't feel like building a proper functor.
	for (auto it = mItems.begin(); it != mItems.end(); ++it)
	{
		if (it->factory == factory)
		{
			mItems.erase(it);
			_update_item_display();
			mCurrentSelection = constants::NO_SELECTION;
			return;
		}
	}

	// basic fuckup check.
	std::cout << "FactoryListBox::removeItem(): annoying bug, fix it." << std::endl;
}


/**
 * Clears all items from the list.
 */
void FactoryListBox::clearItems()
{
	mItems.clear();
	_update_item_display();
}


int FactoryListBox::count() const
{
	return mItems.size();
}


bool FactoryListBox::empty() const
{
	return mItems.empty();
}


int FactoryListBox::currentHighlight() const
{
	return mCurrentHighlight;
}


int FactoryListBox::currentSelection() const
{
	return mCurrentSelection;
}


void FactoryListBox::currentSelection(int selection)
{
	mCurrentSelection = selection;
	mSelectionChanged();
}


const std::string& FactoryListBox::selectionText() const
{
	return mItems[mCurrentSelection].factory->name();
}


/**
 * Resized event handler.
 */
void FactoryListBox::onSizeChanged()
{
	clear();
	add(&mSlider, rect().width() - 14, 0);
	mSlider.displayPosition(false);
	mSlider.size(14, rect().height());
	_update_item_display();
}


/**
 * Visibility changed event handler.
 * 
 * \note	Testing to see if this is an appropriate place to put event connect/disconnect
 *			calls versus testing for visibility in each of the other event handlers.
 */
void FactoryListBox::visibilityChanged(bool visible)
{
	if (!hasFocus()) { return; }
	_hook_events(visible);
}


void FactoryListBox::onFocusChanged()
{
	if (!visible()) { return; }
	_hook_events(hasFocus());
}


/**
 * Mouse Down event handler.
 */
void FactoryListBox::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (empty() || button == EventHandler::BUTTON_MIDDLE) { return; }

	if (button == EventHandler::BUTTON_RIGHT && isPointInRect(x, y, positionX(), positionY(), width(), height()))
	{
		currentSelection(constants::NO_SELECTION);
		return;
	}

	// A few basic checks
	if (!isPointInRect(Point_2d(x, y), rect()) || mCurrentHighlight == constants::NO_SELECTION) { return; }
	if (mSlider.visible() && isPointInRect(Point_2d(x, y), mSlider.rect())) { return; }
	if (mCurrentHighlight < 0 || static_cast<size_t>(mCurrentHighlight) >= mItems.size()) { return; }

	currentSelection(mCurrentHighlight);
}


/**
 * Mouse Motion event handler.
 */
void FactoryListBox::onMouseMove(int x, int y, int relX, int relY)
{
	// Ignore if menu is empty or invisible
	if (empty()) { return; }

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

	mCurrentHighlight = (y - static_cast<int>(positionY()) + mCurrentOffset) / LIST_ITEM_HEIGHT;

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
 * Mouse Wheel event handler.
 * 
 * \todo	Make the scroll step configurable. Legacy from the ListBox.
 */
void FactoryListBox::onMouseWheel(int x, int y)
{
	mSlider.changeThumbPosition((y < 0 ? 16.0 : -16.0));
}


/**
 * Slider changed event handler.
 */
void FactoryListBox::slideChanged(double _position)
{
	_update_item_display();
	int pos = static_cast<int>(_position);
	if (static_cast<float>(pos) != _position)
	{
		mSlider.thumbPosition(static_cast<double>(pos));
	}
}


/**
 * Draws the FactoryListBox
 */
void FactoryListBox::update()
{
	if (!visible()) { return; }

	Renderer& r = Utility<Renderer>::get();

	// CONTROL EXTENTS
	r.drawBoxFilled(rect().x(), rect().y(), mItemWidth, rect().height(), 0, 0, 0, 255);
	r.drawBox(rect().x(), rect().y(), mItemWidth, rect().height(), 75, 75, 75, 255);

	r.clipRect(rect());
	
	// MOUSE HIGHLIGHT
	int highlight_y = positionY() + (mCurrentHighlight * LIST_ITEM_HEIGHT) - mCurrentOffset;
	r.drawBoxFilled(positionX(), highlight_y, mItemWidth, LIST_ITEM_HEIGHT, 0, 185, 0, 50);

	
	// ITEMS
	for (size_t i = 0; i < mItems.size(); ++i)
	{
		drawItem(mItems[i], positionX(), positionY() + (i * LIST_ITEM_HEIGHT), mItemWidth, mCurrentOffset, i == mCurrentSelection);
	}
	

	mSlider.update();		// Shouldn't need this since it's in a UIContainer. Noticing that Slider
							// doesn't play nice with the UIContainer.
	r.clipRectClear();
}
