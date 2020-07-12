#pragma once

#include "UI.h"

#include "NAS2D/Signal.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Renderer/Point.h"
#include "NAS2D/Renderer/Vector.h"
#include "NAS2D/Resources/Image.h"


#include <algorithm>

/**
 * \class IconGrid
 * \brief Represents a 2D grid of Icon items that can be selected with a mouse.
 */
class IconGrid : public Control
{
public:

	/**
	 * Item used within the IconGrid.
	 * 
	 * \note	Have made some changes to IconGrid that effectively makes this class
	 *			completely transparent. Bad for encapsulation but vastly improves
	 *			flexibility in using the IconGrid.
	 */
	class IconGridItem
	{
	public:
		IconGridItem() {}

		std::string name; /**< Name of the Item. Can be empty. */

		int meta = 0; /**< User defined integer value. Optional. */
		bool available = true;

	protected:
		friend class IconGrid;

		NAS2D::Point<int> pos;
	};

	using Callback = NAS2D::Signals::Signal<const IconGridItem*>;

public:
	IconGrid();
	~IconGrid() override;

	void sheetPath(const std::string& filePath);
	void iconSize(int newSsize);
	void iconMargin(int newMargin);

	const std::string& itemName(std::size_t index) const { return mIconItemList[index].name; }

	int selectionIndex() const { return static_cast<int>(mCurrentSelection); }

	bool empty() const { return mIconItemList.empty(); }

	bool sorted() const { return mSorted; }
	void sorted(bool isSorted) { mSorted = isSorted; }

	void addItemSorted(const std::string& name, int sheetIndex, int meta);
	void addItem(const std::string& name, int sheetIndex, int meta);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();

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

	Callback& selectionChanged() { return mCallback; }

	void hide() override;

	void sort();

	void update() override;


protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int dX, int dY);

	virtual void sizeChanged();

private:
	using IconItemList = std::vector<IconGridItem>;
	using Index = IconItemList::size_type;

private:
	void updateGrid();
	std::size_t translateCoordsToIndex(NAS2D::Vector<int> relativeOffset);

	void raiseChangedEvent();

private:
	Index mHighlightIndex = constants::NO_SELECTION; /**< Current highlight index. */
	Index mCurrentSelection = constants::NO_SELECTION; /**< Currently selected item index. */

	int mIconSize = 0; /**< Size of the icons. */
	int mIconMargin = 0; /**< Spacing between icons and edges of the IconGrid. */

	bool mShowTooltip = false; /**< Flag indicating that we want a tooltip drawn near an icon when hovering over it. */
	bool mSorted = true; /**< Flag indicating that the IconGrid should be sorted. */

	NAS2D::Image mIconSheet; /**< Image containing the icons. */

	NAS2D::ImageList mSkin;

	NAS2D::Vector<int> mGridSize; /**< Dimensions of the grid that can be contained in the IconGrid with the current Icon Size and Icon Margin. */

	IconItemList mIconItemList; /**< List of items. */

	Callback mCallback; /**< Callback whenever a selection is made. */
};
