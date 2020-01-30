#pragma once

#include "UI.h"

#include "NAS2D/Signal.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Renderer/Point.h"
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

		std::string name;			/**< Name of the Item. Can be empty. */

		int meta = 0;				/**< User defined integer value. Optional. */
		bool available = true;		/**<  */

	protected:
		friend class IconGrid;

		NAS2D::Point_2df pos;
	};

	typedef NAS2D::Signals::Signal1<const IconGridItem*> Callback;

public:
	IconGrid();
	virtual ~IconGrid();

	void sheetPath(const std::string& _path);
	void iconSize(int _size);
	void iconMargin(int _margin);

	const std::string& itemName(int _sel) const { return mIconItemList[_sel].name; }

	int selectionIndex() const { return static_cast<int>(mCurrentSelection); }

	bool empty() const { return mIconItemList.empty(); }

	bool sorted() const { return mSorted; }
	void sorted(bool _b) { mSorted = _b; }

	void addItemSorted(const std::string& name, int sheetIndex, int meta);
	void addItem(const std::string& name, int sheetIndex, int meta);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();

	// Setter
	void itemAvailable(const std::string& item, bool _b);
	// Getter
	bool itemAvailable(const std::string& item);

	void showTooltip(bool _b) { mShowTooltip = _b; }

	void clearSelection();
	void selection(int _s);
	void selection_meta(int _s);

	void incrementSelection();
	void decrementSelection();

	Callback& selectionChanged() { return mCallback; }

	virtual void hide();

	void sort();

	virtual void update();


protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMotion(int x, int y, int dX, int dY);

	virtual void sizeChanged();

private:
	typedef std::vector<IconGridItem> IconItemList;
	typedef IconItemList::size_type Index;

private:
	void updateGrid();
	int translateCoordsToIndex(int x, int y);

	void raiseChangedEvent();

private:
	Index					mHighlightIndex = constants::NO_SELECTION;		/**< Current highlight index. */
	Index					mCurrentSelection = constants::NO_SELECTION;	/**< Currently selected item index. */

	int					mIconSize = 0;				/**< Size of the icons. */
	int					mIconMargin = 0;			/**< Spacing between icons and edges of the IconGrid. */

	bool				mShowTooltip = false;		/**< Flag indicating that we want a tooltip drawn near an icon when hovering over it. */
	bool				mSorted = true;			/**< Flag indicating that the IconGrid should be sorted. */

	NAS2D::Image		mIconSheet;					/**< Image containing the icons. */

	NAS2D::ImageList	mSkin;

	NAS2D::Point_2d		mGridSize;					/**< Dimensions of the grid that can be contained in the IconGrid with the current Icon Size and Icon Margin. */

	IconItemList		mIconItemList;				/**< List of items. */

	Callback			mCallback;					/**< Callback whenever a selection is made. */
};
