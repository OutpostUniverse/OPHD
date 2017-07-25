#pragma once

#include "UI.h"

#include <algorithm>

/**
 * \class IconGrid
 * \brief Represents a 2D grid of Icon items that can be selected with a mouse.
 */
class IconGrid : public Control
{
public:

	typedef NAS2D::Signals::Signal1<const std::string&> Callback;

	typedef struct IconGridItem {
		IconGridItem() : available(true) {}
		std::string name;
		Point_2df pos;
		bool available;
	} IconGridItem;

	IconGrid();
	virtual ~IconGrid();

	virtual void update();

	void sheetPath(const std::string& _path);
	void iconSize(int _size);
	void iconMargin(int _margin);

	const std::string& itemName(int _sel) const { return mIconItemList[_sel].name; }

	int selectionIndex() const { return mCurrentSelection; }

	bool empty() const { return mIconItemList.empty(); }

	bool sorted() const { return mSorted; }
	void sorted(bool _b) { mSorted = _b; }

	void addItem(const std::string& name, int sheetIndex);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();
	void itemAvailable(const std::string& item, bool _b);

	void showTooltip(bool _b) { mShowTooltip = _b; }

	void clearSelection();
	void selection(int _s);

	void incrementSelection();
	void decrementSelection();

	Callback& selectionChanged() { return mCallback; }

	virtual void hide();

	void sort();

protected:

	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMotion(int x, int y, int dX, int dY);

	virtual void sizeChanged();

private:

	typedef std::vector<IconGridItem> IconItemList;

	void updateGrid();
	int translateCoordsToIndex(int x, int y);

	void raiseChangedEvent();

private:

	int					mHighlightIndex;		/**< Current highlight index. */
	int					mCurrentSelection;		/**< Currently selected item index. */

	int					mIconSize;				/**< Size of the icons. */
	int					mIconMargin;			/**< Spacing between icons and edges of the IconGrid. */

	bool				mShowTooltip;			/**< Flag indicating that we want a tooltip drawn near an icon when hovering over it. */
	bool				mSorted;				/**< Flag indicating that the IconGrid should be sorted. */

	Image				mIconSheet;				/**< Image containing the icons. */

	ImageList			mSkin;

	Point_2d			mGridSize;				/**< Dimensions of the grid that can be contained in the IconGrid with the current Icon Size and Icon Margin. */

	IconItemList		mIconItemList;			/**< List of items. */

	Callback			mCallback;				/**< Callback whenever a selection is made. */
};
