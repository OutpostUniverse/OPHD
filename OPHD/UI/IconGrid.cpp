#include "IconGrid.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/MathUtils.h>

#include <algorithm>
#include <stdexcept>
#include <string>


using namespace NAS2D;


IconGrid::IconGrid(const std::string& filePath, int iconEdgeSize, int margin) :
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	mIconSize{iconEdgeSize},
	mIconMargin{margin},
	mIconSheet{imageCache.load(filePath)},
	mSkin{
		imageCache.load("ui/skin/textbox_top_left.png"),
		imageCache.load("ui/skin/textbox_top_middle.png"),
		imageCache.load("ui/skin/textbox_top_right.png"),
		imageCache.load("ui/skin/textbox_middle_left.png"),
		imageCache.load("ui/skin/textbox_middle_middle.png"),
		imageCache.load("ui/skin/textbox_middle_right.png"),
		imageCache.load("ui/skin/textbox_bottom_left.png"),
		imageCache.load("ui/skin/textbox_bottom_middle.png"),
		imageCache.load("ui/skin/textbox_bottom_right.png")
	}
{
	if (iconEdgeSize <= 0)
	{
		throw std::runtime_error("IconGrid::iconSize must be positive: " + std::to_string(iconEdgeSize));
	}
	if (margin < 0)
	{
		throw std::runtime_error("IconGrid::iconMargin must be non-negative: " + std::to_string(margin));
	}

	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &IconGrid::onMouseDown});
	eventHandler.mouseMotion().connect({this, &IconGrid::onMouseMove});
}


IconGrid::~IconGrid()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &IconGrid::onMouseDown});
	eventHandler.mouseMotion().disconnect({this, &IconGrid::onMouseMove});
}


/**
 * Updates the icon grid.
 */
void IconGrid::updateGrid()
{
	mGridSize = (mRect.size() - NAS2D::Vector{mIconMargin, mIconMargin} * 2) / (mIconSize + mIconMargin);
}


void IconGrid::onMouseDown(EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	// Don't respond to anything unless it's the left mouse button.
	if (button != EventHandler::MouseButton::Left) { return; }

	if (!visible())
	{
		return;
	}

	auto startPoint = mRect.startPoint();
	if (mIconItemList.empty() || !NAS2D::Rectangle<int>::Create(startPoint, mGridSize * (mIconSize + mIconMargin)).contains(position))
	{
		return;
	}

	auto previousIndex = mSelectedIndex;
	mSelectedIndex = translateCoordsToIndex(position - startPoint);

	if (mSelectedIndex >= mIconItemList.size())
	{
		mSelectedIndex = constants::NoSelection;
	}

	if (previousIndex != mSelectedIndex)
	{
		raiseChangedEvent();
	}
}


void IconGrid::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (!enabled() || !visible()) { return; }

	auto startPoint = mRect.startPoint();
	if (mIconItemList.empty() || !NAS2D::Rectangle<int>::Create(startPoint, mGridSize * (mIconSize + mIconMargin)).contains(position))
	{
		mHighlightIndex = constants::NoSelection;
		return;
	}

	// Assumes all coordinates are not negative.
	mHighlightIndex = translateCoordsToIndex(position - startPoint);

	if (mHighlightIndex >= mIconItemList.size())
	{
		mHighlightIndex = constants::NoSelection;
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
void IconGrid::onResize()
{
	Control::onResize();

	updateGrid();
}


void IconGrid::addItem(const Item& item)
{
	mIconItemList.push_back(item);

	const int x_pos = (item.sheetId % (mIconSheet.size().x / mIconSize)) * mIconSize;
	const int y_pos = (item.sheetId / (mIconSheet.size().x / mIconSize)) * mIconSize;

	mIconItemList.back().pos = {x_pos, y_pos};
}


/**
 * Set item availability
 */
void IconGrid::itemAvailable(const std::string& item, bool isItemAvailable)
{
	for (auto& iconItem : mIconItemList)
	{
		if (iconItem.name == item)
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
	for (const auto& iconItem : mIconItemList)
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

	const auto iter = std::find_if(
		mIconItemList.begin(),
		mIconItemList.end(),
		[&lowerCaseTarget](const auto& iconItem) {
			return toLowercase(iconItem.name) == lowerCaseTarget;
		}
	);
	if (iter != mIconItemList.end())
	{
		mIconItemList.erase(iter);
		clearSelection();
	}
}


/**
 * Indicates wether a named item exists in the IconGrid.
 */
bool IconGrid::itemExists(const std::string& item)
{
	const auto lowerCaseTarget = toLowercase(item);
	for (const auto& iconItem : mIconItemList)
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
void IconGrid::clear()
{
	mIconItemList.clear();
	clearSelection();
}


/**
 * Clears the Highlight Index and the Selection Index.
 */
void IconGrid::clearSelection()
{
	mHighlightIndex = constants::NoSelection;
	mSelectedIndex = constants::NoSelection;
}


/**
 * Sets the current selection index.
 */
void IconGrid::selection(std::size_t newSelection)
{
	mSelectedIndex = (newSelection < mIconItemList.size()) ? newSelection : constants::NoSelection;
}


/**
 * Sets the current selection index to the first item with
 * a matching meta value.
 * 
 * \note	If no items in the list contain a matching meta value,
 *			the current selection index will remain unchanged.
 * 
 * \remark	IconGrid::Item::meta is initialized to 0 so passing a value
 *			of 0 for IconGrid::Item's that don't use a meta value will
 *			effectively set the index to 0.
 */
void IconGrid::selection_meta(int selectionMetaValue)
{
	for (std::size_t i = 0; i < mIconItemList.size(); ++i)
	{
		if (mIconItemList[i].meta == selectionMetaValue)
		{
			mSelectedIndex = i;
			return;
		}
	}
}


void IconGrid::incrementSelection()
{
	++mSelectedIndex;
	if (mSelectedIndex >= mIconItemList.size())
	{
		mSelectedIndex = 0;
	}

	if (mIconItemList.empty())
	{
		mSelectedIndex = constants::NoSelection;
	}

	raiseChangedEvent();
}


void IconGrid::decrementSelection()
{
	if (mSelectedIndex == 0)
	{
		mSelectedIndex = mIconItemList.size();
	}
	--mSelectedIndex;

	if (mIconItemList.empty())
	{
		mSelectedIndex = constants::NoSelection;
	}

	raiseChangedEvent();
}


void IconGrid::raiseChangedEvent()
{
	if (mSelectedIndex != constants::NoSelection)
	{
		mSignal(&mIconItemList[mSelectedIndex]);
	}
	else
	{
		mSignal(nullptr);
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

	mSkin.draw(renderer, mRect);

	if (mGridSize.x == 0) { return; }
	const auto indexToGridPosition = [gridSize = mGridSize, startPoint = mRect.startPoint(), spacing = mIconSize + mIconMargin](std::size_t index) {
		const auto linearOffset = static_cast<int>(index);
		const auto offset = NAS2D::Vector{linearOffset % gridSize.x, linearOffset / gridSize.x};
		return startPoint + offset * spacing;
	};

	for (std::size_t i = 0; i < mIconItemList.size(); ++i)
	{
		const auto position = indexToGridPosition(i);
		const auto highlightColor = mIconItemList[i].available ? NAS2D::Color::White : NAS2D::Color::Red;
		renderer.drawSubImage(mIconSheet, position, NAS2D::Rectangle{mIconItemList[i].pos.x, mIconItemList[i].pos.y, mIconSize, mIconSize}, highlightColor);
	}

	if (mSelectedIndex != constants::NoSelection)
	{
		const auto position = indexToGridPosition(mSelectedIndex) + NAS2D::Vector{mIconMargin, mIconMargin};
		renderer.drawBox(NAS2D::Rectangle<int>::Create(position, NAS2D::Vector{mIconSize, mIconSize}), NAS2D::Color{0, 100, 255});
	}

	if (mHighlightIndex != constants::NoSelection)
	{
		const auto position = indexToGridPosition(mHighlightIndex) + NAS2D::Vector{mIconMargin, mIconMargin};
		renderer.drawBox(NAS2D::Rectangle<int>::Create(position, NAS2D::Vector{mIconSize, mIconSize}), NAS2D::Color{0, 180, 0});

		// Name Tooltip
		if (mShowTooltip)
		{
			const auto& highlightedName = mIconItemList[mHighlightIndex].name;
			const auto tooltipRect = NAS2D::Rectangle{position.x, position.y - 15, mFont.width(highlightedName) + 4, mFont.height()};
			renderer.drawBoxFilled(tooltipRect, NAS2D::Color{245, 245, 245});
			renderer.drawBox(tooltipRect, NAS2D::Color{175, 175, 175});
			renderer.drawText(mFont, highlightedName, position + NAS2D::Vector{2, -15}, NAS2D::Color::Black);
		}
	}
}


void IconGrid::sort()
{
	const auto iconItemCompare = [](const auto& left, const auto& right){ return left.name < right.name; };
	std::sort(mIconItemList.begin(), mIconItemList.end(), iconItemCompare);
}
