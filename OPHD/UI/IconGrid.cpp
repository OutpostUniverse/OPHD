// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "IconGrid.h"

#include "../Constants.h"
#include "../FontManager.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/MathUtils.h>

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
	int cols = (mRect.width - (mIconMargin * 2)) / (mIconSize + mIconMargin);
	int rows = (mRect.height - (mIconMargin * 2)) / (mIconSize + mIconMargin);

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

	auto startPoint = mRect.startPoint();
	auto mousePoint = NAS2D::Point{x, y};
	if (mIconItemList.empty() || !NAS2D::Rectangle<int>::Create(startPoint, mGridSize * (mIconSize + mIconMargin)).contains(mousePoint))
	{
		return;
	}

	mCurrentSelection = translateCoordsToIndex(mousePoint - startPoint);

	if (mCurrentSelection >= mIconItemList.size())
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

	auto startPoint = mRect.startPoint();
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
std::size_t IconGrid::translateCoordsToIndex(NAS2D::Vector<int> relativeOffset)
{
	const auto gridOffset = relativeOffset / (mIconSize + mIconMargin);
	return static_cast<std::size_t>(gridOffset.x + (mGridSize.x * gridOffset.y));
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
	int x_pos = (sheetIndex % (mIconSheet.size().x / mIconSize)) * mIconSize;
	int y_pos = (sheetIndex / (mIconSheet.size().x / mIconSize)) * mIconSize;

	mIconItemList.push_back(IconGridItem());

	IconGrid::IconGridItem& _item = mIconItemList.back();

	_item.name = name;
	_item.pos = {x_pos, y_pos};
	_item.meta = meta;
}


/**
 * Set item availability
 */
void IconGrid::itemAvailable(const std::string& item, bool isItemAvailable)
{
	const auto lowerCaseTarget = toLowercase(item);
	for (auto &iconItem : mIconItemList)
	{
		if (toLowercase(iconItem.name) == lowerCaseTarget)
		{
			iconItem.available = isItemAvailable;
			return;
		}
	}
}


/**
 * Get item availability
 */
bool IconGrid::itemAvailable(const std::string& item)
{
	const auto lowerCaseTarget = toLowercase(item);
	for (auto &iconItem : mIconItemList)
	{
		if (toLowercase(iconItem.name) == lowerCaseTarget)
		{
			return iconItem.available;
		}
	}
	return false;
}


/**
 * Removes an item from the IconGrid by name.
 */
void IconGrid::removeItem(const std::string& item)
{
	const auto lowerCaseTarget = toLowercase(item);

	auto it = mIconItemList.begin();
	while (it != mIconItemList.end())
	{
		if (toLowercase((*it).name) == lowerCaseTarget)
		{
			mIconItemList.erase(it);
			clearSelection();
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
	const auto lowerCaseTarget = toLowercase(item);
	for (auto &iconItem : mIconItemList)
	{
		if (toLowercase(iconItem.name) == lowerCaseTarget)
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
void IconGrid::selection(std::size_t newSelection)
{
	mCurrentSelection = (newSelection < mIconItemList.size()) ? newSelection : constants::NO_SELECTION;
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
	if (mCurrentSelection >= mIconItemList.size())
	{
		mCurrentSelection = 0;
	}

	if (mIconItemList.empty())
	{
		mCurrentSelection = constants::NO_SELECTION;
	}

	raiseChangedEvent();
}


void IconGrid::decrementSelection()
{
	if (mCurrentSelection == 0)
	{
		mCurrentSelection = mIconItemList.size();
	}
	--mCurrentSelection;

	if (mIconItemList.empty())
	{
		mCurrentSelection = constants::NO_SELECTION;
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

	auto& renderer = Utility<Renderer>::get();

	renderer.drawImageRect(mRect, mSkin);

	if (mGridSize.x == 0) { return; }
	const auto indexToGridPosition = [gridSize = mGridSize, startPoint = mRect.startPoint(), spacing = mIconSize + mIconMargin](std::size_t index) {
		const auto linearOffset = static_cast<int>(index);
		const auto offset = NAS2D::Vector{linearOffset % gridSize.x, linearOffset / gridSize.x};
		return startPoint + offset * spacing;
	};

	if (mIconItemList.empty()) { return; }

	for (std::size_t i = 0; i < mIconItemList.size(); ++i)
	{
		const auto position = indexToGridPosition(i);
		const auto highlightColor = mIconItemList[i].available ? NAS2D::Color::White : NAS2D::Color::Red;
		renderer.drawSubImage(mIconSheet, position, NAS2D::Rectangle{mIconItemList[i].pos.x, mIconItemList[i].pos.y, mIconSize, mIconSize}, highlightColor);
	}

	if (mCurrentSelection != constants::NO_SELECTION)
	{
		const auto position = indexToGridPosition(mCurrentSelection) + NAS2D::Vector{mIconMargin, mIconMargin};
		renderer.drawBox(NAS2D::Rectangle<int>::Create(position, NAS2D::Vector{mIconSize, mIconSize}), NAS2D::Color{0, 100, 255});
	}

	if (mHighlightIndex != constants::NO_SELECTION)
	{
		const auto position = indexToGridPosition(mHighlightIndex) + NAS2D::Vector{mIconMargin, mIconMargin};
		renderer.drawBox(NAS2D::Rectangle<int>::Create(position, NAS2D::Vector{mIconSize, mIconSize}), NAS2D::Color{0, 180, 0});

		// Name Tooltip
		if (mShowTooltip)
		{
			const auto tooltipRect = NAS2D::Rectangle{position.x, position.y - 15, FONT->width(mIconItemList[mHighlightIndex].name) + 4, FONT->height()};
			renderer.drawBoxFilled(tooltipRect, NAS2D::Color{245, 245, 245});
			renderer.drawBox(tooltipRect, NAS2D::Color{175, 175, 175});
			renderer.drawText(*FONT, mIconItemList[mHighlightIndex].name, position + NAS2D::Vector{2, -15}, NAS2D::Color::Black);
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
