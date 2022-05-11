#pragma once

#include "Core/Control.h"

#include "../Constants/UiConstants.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/StringUtils.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/RectangleSkin.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/Image.h>

#include <algorithm>


/**
 * Represents a 2D grid of Icon items that can be selected with a mouse.
 */
class IconGrid : public Control
{
public:

	/**
	 * Item used within the IconGrid.
	 */
	struct Item
	{
	public:
		std::string name{};
		int sheetId{0};
		int meta{0}; /**< Optional User defined value */
		bool available{true};
		NAS2D::Point<int> pos{};
	};

	using Signal = NAS2D::Signal<const Item*>;

public:
	IconGrid(const std::string& filePath, int iconSize, int margin);
	~IconGrid() override;

	const std::string& itemName(std::size_t index) const { return mIconItemList[index].name; }

	int selectionIndex() const { return static_cast<int>(mSelectedIndex); }

	bool empty() const { return mIconItemList.empty(); }

	void addItem(const Item&);

	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void clear();

	// Setter
	void itemAvailable(const std::string& item, bool isItemAvailable);
	// Getter
	bool itemAvailable(const std::string& item);

	void showTooltip(bool value) { mShowTooltip = value; }

	void clearSelection();
	void selection(std::size_t newSelection);
	void selection_meta(int selectionMetaValue);

	void incrementSelection();
	void decrementSelection();

	Signal::Source& selectionChanged() { return mSignal; }

	void hide() override;

	void sort();

	void update() override;


protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int dX, int dY);

	void onResize() override;

private:
	using IconItemList = std::vector<Item>;
	using Index = IconItemList::size_type;

	void updateGrid();
	std::size_t translateCoordsToIndex(NAS2D::Vector<int> relativeOffset);

	void raiseChangedEvent();

	const NAS2D::Font& mFont;

	Index mHighlightIndex = constants::NoSelection; /**< Current highlight index. */
	Index mSelectedIndex = constants::NoSelection; /**< Currently selected item index. */

	int mIconSize = 1; /**< Size of the icons. */
	int mIconMargin = 0; /**< Spacing between icons and edges of the IconGrid. */

	bool mShowTooltip = false; /**< Flag indicating that we want a tooltip drawn near an icon when hovering over it. */

	const NAS2D::Image& mIconSheet; /**< Image containing the icons. */

	NAS2D::RectangleSkin mSkin;

	NAS2D::Vector<int> mGridSize; /**< Dimensions of the grid that can be contained in the IconGrid with the current Icon Size and Icon Margin. */

	IconItemList mIconItemList; /**< List of items. */

	Signal mSignal; /**< Signal whenever a selection is made. */
};
