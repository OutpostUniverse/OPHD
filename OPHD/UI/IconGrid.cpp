// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "IconGrid.h"

#include "../Constants.h"
#include "../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"
#include "NAS2D/MathUtils.h"

using namespace NAS2D;

static Font* FONT = nullptr;

/**
 * C'tor
 */
IconGrid::IconGrid()
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &IconGrid::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().connect(this, &IconGrid::onMouseMove);
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

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
}


/**
 * D'tor
 */
IconGrid::~IconGrid()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &IconGrid::onMouseDown);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &IconGrid::onMouseMove);
}


/**
 * Sets the icon sheet path and loads an Image.
 */
void IconGrid::sheetPath(const std::string& filePath)
{
	mIconSheet = Image(filePath);
}


/**
 * Sets the icon dimensions.
 */
void IconGrid::iconSize(int newSsize)
{
	mIconSize = newSsize;
	updateGrid();
}


/**
 * Sets the margin used between the edges of the IconGrid and other icons.
 */
void IconGrid::iconMargin(int newMargin)
{
	mIconMargin = newMargin;
	updateGrid();
}


/**
 * Updates the icon grid.
 */
void IconGrid::updateGrid()
{
	// explicit cast, intentional trunctation of floating point data.
	int cols = static_cast<int>((rect().width() - (mIconMargin * 2)) / (mIconSize + mIconMargin));
	int rows = static_cast<int>((rect().height() - (mIconMargin * 2)) / (mIconSize + mIconMargin));

	mGridSize = {cols, rows};
}


/**
 * MouseDown event handler.
 */
void IconGrid::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!visible() || !hasFocus()) { return; }

	// Don't respond to anything unless it's the left mouse button.
	if (button != EventHandler::MouseButton::BUTTON_LEFT) { return; }

	if (!visible())
	{
		return;
	}

	auto startPoint = mRect.startPoint().to<int>();
	auto mousePoint = NAS2D::Point{x, y};
	if (mIconItemList.empty() || !NAS2D::Rectangle<int>::Create(startPoint, mGridSize * (mIconSize + mIconMargin)).contains(mousePoint))
	{
		return;
	}

	mCurrentSelection = translateCoordsToIndex(mousePoint - startPoint);

	if (static_cast<std::size_t>(mCurrentSelection) >= mIconItemList.size())
	{
		mCurrentSelection = constants::NO_SELECTION;
	}
	/* else if (!mIconItemList[mCurrentSelection].available)
	{
		cout << "Insufficient resources" <<endl;
		mCurrentSelection = constants::NO_SELECTION;
		return;
	} */

	raiseChangedEvent();
}


/**
 * MouseMotion event handler.
 */
void IconGrid::onMouseMove(int x, int y, int /*dX*/, int /*dY*/)
{
	if (!visible() || !hasFocus()) { return; }

	auto startPoint = mRect.startPoint().to<int>();
	auto mousePoint = NAS2D::Point{x, y};
	if (mIconItemList.empty() || !NAS2D::Rectangle<int>::Create(startPoint, mGridSize * (mIconSize + mIconMargin)).contains(mousePoint))
	{
		mHighlightIndex = constants::NO_SELECTION;
		return;
	}

	// Assumes all coordinates are not negative.
	mHighlightIndex = translateCoordsToIndex(mousePoint - startPoint);

	if (static_cast<std::size_t>(mHighlightIndex) >= mIconItemList.size())
	{
		mHighlightIndex = constants::NO_SELECTION;
	}
}


/**
 * Utility function that translates mouse coordinates into
 * an index value.
 */
int IconGrid::translateCoordsToIndex(NAS2D::Vector<int> relativeOffset)
{
	const auto gridOffset = relativeOffset / (mIconSize + mIconMargin);
	return gridOffset.x + (mGridSize.x * gridOffset.y);
}


/**
 * Called whenever the size of the IconGrid is changed.
 */
void IconGrid::sizeChanged()
{
	updateGrid();
}


/**
 * Adds an Icon Item to the IconGrid and sorts the list after addition.
 * 
 * \param	meta	User defined integer value. Defaults to 0.
 *
 * \note	This function does no sanity checking.
 */
void IconGrid::addItemSorted(const std::string& name, int sheetIndex, int meta)
{
	addItem(name, sheetIndex, meta);
	sort();
}


/**
 * Adds an Icon Item to the IconGrid.
 * 
 * \param	meta	User defined integer value. Defaults to 0.
 *
 * \note	This function does no sanity checking.
 */
void IconGrid::addItem(const std::string& name, int sheetIndex, int meta)
{
	int x_pos = (sheetIndex % (mIconSheet.width() / mIconSize)) * mIconSize;
	int y_pos = (sheetIndex / (mIconSheet.width() / mIconSize)) * mIconSize;

	mIconItemList.push_back(IconGridItem());

	IconGrid::IconGridItem& _item = mIconItemList.back();

	_item.name = name;
	_item.pos = {static_cast<float>(x_pos), static_cast<float>(y_pos)};
	_item.meta = meta;
}


/**
 * Set item availability
 */
void IconGrid::itemAvailable(const std::string& item, bool isItemAvailable)
{
	// Ignore if menu is empty
	if (empty())
	{
		return;
	}

	for (auto &_iconItem : mIconItemList)
	{
		if (toLowercase(_iconItem.name) == toLowercase(item))
		{
			_iconItem.available = isItemAvailable;
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
	{
		return false;
	}

	for (auto &_iconItem : mIconItemList)
	{
		if (toLowercase(_iconItem.name) == toLowercase(item))
		{
			return _iconItem.available;
		}
	}
	return false;
}


/**
 * Removes an item from the IconGrid by name.
 */
void IconGrid::removeItem(const std::string& item)
{
	if (empty())
	{
		return;
	}

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
	{
		return false;
	}

	for (std::size_t i = 0; i < mIconItemList.size(); i++)
	{
		if (toLowercase(mIconItemList[i].name) == toLowercase(item))
		{
			return true;
		}
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
void IconGrid::selection(int newSelection)
{
	if (static_cast<std::size_t>(newSelection) >= mIconItemList.size())
		return;

	mCurrentSelection = newSelection;
}


/**
 * Sets the current selection index to the first item with
 * a matching meta value.
 * 
 * \note	If no items in the list contain a matching meta value,
 *			the current selection index will remain unchanged.
 * 
 * \remark	IconGridItem::meta is initialized to 0 so passing a value
 *			of 0 for IconGridItem's that don't use a meta value will
 *			effectively set the index to 0.
 */
void IconGrid::selection_meta(int selectionMetaValue)
{
	for (std::size_t i = 0; i < mIconItemList.size(); ++i)
	{
		if (mIconItemList[i].meta == selectionMetaValue)
		{
			mCurrentSelection = i;
			return;
		}
	}
}


void IconGrid::incrementSelection()
{
	++mCurrentSelection;

	if (static_cast<std::size_t>(mCurrentSelection) >= mIconItemList.size())
		mCurrentSelection = 0;

	raiseChangedEvent();
}


void IconGrid::decrementSelection()
{
	if (mCurrentSelection == 0)
	{
		mCurrentSelection = mIconItemList.size() - 1;
	}
	else
	{
		--mCurrentSelection;
	}

	raiseChangedEvent();
}


void IconGrid::raiseChangedEvent()
{
	if (mCurrentSelection != constants::NO_SELECTION)
	{
		mCallback(&mIconItemList[mCurrentSelection]);
	}
	else
	{
		mCallback(nullptr);
	}
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
	if (!visible()) { return; }

	Renderer& r = Utility<Renderer>::get();

	//r.drawBoxFilled(rect(), 0, 0, 0);
	r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkin);

	if (mIconItemList.empty()) { return; }

	for (std::size_t i = 0; i < mIconItemList.size(); ++i)
	{
		int x_pos = static_cast<int>(i) % mGridSize.x;
		int y_pos = static_cast<int>(i) / mGridSize.x; //-V537

		float x = static_cast<float>((rect().x() + mIconMargin) + (x_pos * mIconSize) + (mIconMargin * x_pos));
		float y = static_cast<float>((rect().y() + mIconMargin) + (y_pos * mIconSize) + (mIconMargin * y_pos));

		if (mIconItemList[i].available)
			r.drawSubImage(mIconSheet, x, y, mIconItemList[i].pos.x(), mIconItemList[i].pos.y(), static_cast<float>(mIconSize), static_cast<float>(mIconSize));
		else
			r.drawSubImage(mIconSheet, x, y, mIconItemList[i].pos.x(), mIconItemList[i].pos.y(), static_cast<float>(mIconSize), static_cast<float>(mIconSize), 255, 0, 0, 255);
	}

	if (mCurrentSelection != constants::NO_SELECTION)
	{
		int x_pos = (static_cast<int>(mCurrentSelection) % mGridSize.x);
		int y_pos = (static_cast<int>(mCurrentSelection) / mGridSize.x); //-V537
		r.drawBox((rect().x() + mIconMargin) + (x_pos * mIconSize) + (mIconMargin * x_pos),
			(rect().y() + mIconMargin) + (y_pos * mIconSize) + (mIconMargin * y_pos),
			static_cast<float>(mIconSize),
			static_cast<float>(mIconSize), 0, 100, 255);
	}

	if (mHighlightIndex != constants::NO_SELECTION)
	{
		int x_pos = (static_cast<int>(mHighlightIndex) % mGridSize.x);
		int y_pos = (static_cast<int>(mHighlightIndex) / mGridSize.x); //-V537

		int x = static_cast<int>((rect().x() + mIconMargin) + (x_pos * mIconSize) + (mIconMargin * x_pos));
		int y = static_cast<int>((rect().y() + mIconMargin) + (y_pos * mIconSize) + (mIconMargin * y_pos));

		r.drawBox(static_cast<float>(x), static_cast<float>(y), static_cast<float>(mIconSize), static_cast<float>(mIconSize), 0, 180, 0);

		// Name Tooltip
		if (mShowTooltip)
		{
			r.drawBoxFilled(static_cast<float>(x),
							static_cast<float>(y - 15),
							static_cast<float>(FONT->width(mIconItemList[mHighlightIndex].name) + 4),
							static_cast<float>(FONT->height()), 245, 245, 245);
			
			r.drawBox(	static_cast<float>(x),
						static_cast<float>(y - 15),
						static_cast<float>(FONT->width(mIconItemList[mHighlightIndex].name) + 4),
						static_cast<float>(FONT->height()), 175, 175, 175);
			
			r.drawText(*FONT, mIconItemList[mHighlightIndex].name, static_cast<float>(x + 2), static_cast<float>(y - 15), 0, 0, 0);
		}
	}
}


bool iconItemCompare(const IconGrid::IconGridItem& left, const IconGrid::IconGridItem& right)
{
	return left.name < right.name;
}


void IconGrid::sort()
{
	if (sorted())
	{
		std::sort(mIconItemList.begin(), mIconItemList.end(), &iconItemCompare);
	}
}
