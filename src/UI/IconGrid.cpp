#include "IconGrid.h"

#include "../Constants.h"


/**
 * C'tor
 */
IconGrid::IconGrid():	mHighlightIndex(constants::NO_SELECTION),
						mCurrentSelection(constants::NO_SELECTION),
						mIconSize(0),
						mIconMargin(0),
						mShowTooltip(false),
						mSorted(true)
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &IconGrid::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().connect(this, &IconGrid::onMouseMotion);
	hasFocus(true);

	mSkin.push_back(Image("ui/skin/textbox_top_left.png"));
	mSkin.push_back(Image("ui/skin/textbox_top_middle.png"));
	mSkin.push_back(Image("ui/skin/textbox_top_right.png"));
	mSkin.push_back(Image("ui/skin/textbox_middle_left.png"));
	mSkin.push_back(Image("ui/skin/textbox_middle_middle.png"));
	mSkin.push_back(Image("ui/skin/textbox_middle_right.png"));
	mSkin.push_back(Image("ui/skin/textbox_bottom_left.png"));
	mSkin.push_back(Image("ui/skin/textbox_bottom_middle.png"));
	mSkin.push_back(Image("ui/skin/textbox_bottom_right.png"));
}


/**
 * D'tor
 */
IconGrid::~IconGrid()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &IconGrid::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &IconGrid::onMouseMotion);
}


/**
 * Sets the icon sheet path and loads an Image.
 */
void IconGrid::sheetPath(const std::string& _path)
{
	mIconSheet = Image(_path);
}


/**
 * Sets the icon dimensions.
 */
void  IconGrid::iconSize(int _size)
{
	mIconSize = _size;
	updateGrid();
}


/**
 * Sets the margin used between the edges of the IconGrid and other icons.
 */
void IconGrid::iconMargin(int _margin)
{
	mIconMargin = _margin;
	updateGrid();
}


/**
 * Updates the icon grid.
 */
void IconGrid::updateGrid()
{
	int cols = (rect().width() - (mIconMargin * 2)) / (mIconSize + mIconMargin);
	int rows = (rect().height() - (mIconMargin * 2)) / (mIconSize + mIconMargin);

	mGridSize(cols, rows);
}


/**
 * MouseDown event handler.
 */
void IconGrid::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!visible() || !hasFocus())
		return;

	// Don't respond to anything unless it's the left mouse button.
	if (button != EventHandler::BUTTON_LEFT)
		return;

	if (!visible())
		return;

	if (mIconItemList.empty() || !isPointInRect(x, y, rect().x(), rect().y(), mGridSize.x() * (mIconSize + mIconMargin), mGridSize.y() * (mIconSize + mIconMargin)))
		return;

	mCurrentSelection = translateCoordsToIndex(x - rect().x(), y - rect().y());

	if (mCurrentSelection >= mIconItemList.size()) {
		mCurrentSelection = constants::NO_SELECTION;
	}/* else if (!mIconItemList[mCurrentSelection].available) {
		cout << "Insufficient resources" <<endl;
		mCurrentSelection = constants::NO_SELECTION;
		return;
	} */

	raiseChangedEvent();
}


/**
 * MouseMotion event handler.
 */
void IconGrid::onMouseMotion(int x, int y, int dX, int dY)
{
	if (!visible() || !hasFocus())
		return;

	if (mIconItemList.empty() || !isPointInRect(x, y, rect().x(), rect().y(), mGridSize.x() * (mIconSize + mIconMargin), mGridSize.y() * (mIconSize + mIconMargin)))
	{
		mHighlightIndex = constants::NO_SELECTION;
		return;
	}

	// Assumes all coordinates are not negative.
	mHighlightIndex = translateCoordsToIndex(x - rect().x(), y - rect().y());

	if (mHighlightIndex >= mIconItemList.size())
		mHighlightIndex = constants::NO_SELECTION;
}


/**
 * Utility function that translates mouse coordinates into
 * an index value.
 */
int IconGrid::translateCoordsToIndex(int x, int y)
{
	return (x / (mIconSize + mIconMargin)) + (mGridSize.x() * (y / (mIconSize + mIconMargin)));
}


/**
 * Called whenever the size of the IconGrid is changed.
 */
void IconGrid::sizeChanged()
{
	updateGrid();
}


/**
 * Adds an Icon Item to the IconGrid.
 *
 * \note	This function does no sanity checking.
 */
void IconGrid::addItem(const std::string& name, int sheetIndex)
{
	int x_pos = (sheetIndex % (mIconSheet.width() / mIconSize)) * mIconSize;
	int y_pos = (sheetIndex / (mIconSheet.width() / mIconSize)) * mIconSize;

	mIconItemList.push_back(IconGridItem());
	mIconItemList.back().name = name;
	mIconItemList.back().pos((float)x_pos, (float)y_pos);

	sort();
}


/**
 * Set item availability
 */
void IconGrid::itemAvailable(const std::string& item, bool _b)
{
	// Ignore if menu is empty
	if (empty())
		return;

	for (auto &_iconItem : mIconItemList)
	{
		if (toLowercase(_iconItem.name) == toLowercase(item)) {
			_iconItem.available = _b;
			return;
		}
	}
}


/**
 * Get item availability
 */
bool IconGrid::itemAvailable(const std::string& item)
{
	// Ignore if menu is empty
	if (empty())
		return false;

	for (auto &_iconItem : mIconItemList)
	{
		if (toLowercase(_iconItem.name) == toLowercase(item))
			return _iconItem.available;
	}
	return false;
}


/**
 * Removes an item from the IconGrid by name.
 */
void IconGrid::removeItem(const std::string& item)
{
	if (empty())
		return;

	auto it = mIconItemList.begin();

	while (it != mIconItemList.end())
	{
		if (toLowercase((*it).name) == toLowercase(item))
		{
			mIconItemList.erase(it);
			mCurrentSelection = constants::NO_SELECTION;
			clearSelection();
			sort();
			return;
		}

		++it;
	}
}


/**
 * Indicates wether a named item exists in the IconGrid.
 */
bool IconGrid::itemExists(const std::string& item)
{
	// Ignore if menu is empty
	if (empty())
		return false;

	for (size_t i = 0; i < mIconItemList.size(); i++)
	{
		if (toLowercase(mIconItemList[i].name) == toLowercase(item))
			return true;
	}

	return false;
}


/**
 * Drops all items from the IconGrid.
 */
void IconGrid::dropAllItems()
{
	mIconItemList.clear();
	clearSelection();
}


/**
 * Clears the Highlight Index and the Selection Index.
 */
void IconGrid::clearSelection()
{
	mHighlightIndex = constants::NO_SELECTION;
	mCurrentSelection = constants::NO_SELECTION;
}


/**
 * Sets the current selection index.
 */
void IconGrid::selection(int _s)
{
	if (_s >= mIconItemList.size())
		return;

	mCurrentSelection = _s;
}


void IconGrid::incrementSelection()
{
	++mCurrentSelection;

	if (mCurrentSelection >= mIconItemList.size())
		mCurrentSelection = 0;

	raiseChangedEvent();
}


void IconGrid::decrementSelection()
{
	--mCurrentSelection;

	if (mCurrentSelection < 0)
		mCurrentSelection = mIconItemList.size() - 1;

	raiseChangedEvent();
}


void IconGrid::raiseChangedEvent()
{
	if (mCurrentSelection != constants::NO_SELECTION)
		mCallback(mIconItemList[mCurrentSelection].name);
	else
		mCallback("");
}

/**
 * Hide override -- clears selections whenever IconGrid is hidden.
 */
void IconGrid::hide()
{
	Control::hide();
	clearSelection();
}


/**
 * Draws the IconGrid.
 */
void IconGrid::update()
{
	if (!visible())
		return;

	Renderer& r = Utility<Renderer>::get();

	//r.drawBoxFilled(rect(), 0, 0, 0);
	r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkin);

	if (mIconItemList.empty())
		return;

	for (size_t i = 0; i < mIconItemList.size(); ++i)
	{
		int x_pos = i % mGridSize.x();
		int y_pos = i / mGridSize.x();

		float x = (rect().x() + mIconMargin) + (x_pos * mIconSize) + (mIconMargin * x_pos);
		float y = (rect().y() + mIconMargin) + (y_pos * mIconSize) + (mIconMargin * y_pos);

		if (mIconItemList[i].available)
			r.drawSubImage(mIconSheet, x, y, mIconItemList[i].pos.x(), mIconItemList[i].pos.y(), mIconSize, mIconSize);
		else
			r.drawSubImage(mIconSheet, x, y, mIconItemList[i].pos.x(), mIconItemList[i].pos.y(), mIconSize, mIconSize, 255, 0, 0, 255);
	}

	if (mCurrentSelection != constants::NO_SELECTION)
	{
		int x_pos = (mCurrentSelection % mGridSize.x());
		int y_pos = (mCurrentSelection / mGridSize.x());
		r.drawBox((rect().x() + mIconMargin) + (x_pos * mIconSize) + (mIconMargin * x_pos), (rect().y() + mIconMargin) + (y_pos * mIconSize) + (mIconMargin * y_pos), mIconSize, mIconSize, 0, 100, 255);
	}

	if (mHighlightIndex != constants::NO_SELECTION)
	{
		int x_pos = (mHighlightIndex % mGridSize.x());
		int y_pos = (mHighlightIndex / mGridSize.x());

		int x = (rect().x() + mIconMargin) + (x_pos * mIconSize) + (mIconMargin * x_pos);
		int y = (rect().y() + mIconMargin) + (y_pos * mIconSize) + (mIconMargin * y_pos);

		r.drawBox(x, y, mIconSize, mIconSize, 0, 180, 0);

		// Name Tooltip
		if (mShowTooltip)
		{
			r.drawBoxFilled(x, y - 15, font().width(mIconItemList[mHighlightIndex].name) + 4, font().height(), 245, 245, 245);
			r.drawBox(x, y - 15, font().width(mIconItemList[mHighlightIndex].name) + 4, font().height(), 175, 175, 175);
			r.drawText(font(), mIconItemList[mHighlightIndex].name, x + 2, y - 15, 0, 0, 0);
		}
	}
}


bool iconItemCompare(const IconGrid::IconGridItem& left, const IconGrid::IconGridItem& right)
{
	return left.name < right.name;
}


void IconGrid::sort()
{
	if(sorted())
		std::sort(mIconItemList.begin(), mIconItemList.end(), &iconItemCompare);
}
