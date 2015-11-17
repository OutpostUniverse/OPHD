#pragma once

#include "UI.h"


/**
 * \class IconGrid
 * \brief Represents a 2D grid of Icon items that can be selected with a mouse.
 */
class IconGrid : public Control
{
public:

	typedef Gallant::Signal1<const std::string&> Callback;

	IconGrid();
	virtual ~IconGrid();

	virtual void update();

	void sheetPath(const std::string& _path);
	void iconSize(int _size);
	void iconMargin(int _margin);

	const std::string& itemName(int _sel) const { return mIconItemList[_sel]._name; }

	bool empty() const { return mIconItemList.empty(); }

	void addItem(const std::string& name, int sheetIndex);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();

	void showTooltip(bool _b) { mShowTooltip = _b; }

	void clearSelection();
	void selection(int _s);

	Callback& selectionChanged() { return mCallback; }

	virtual void hide();

protected:

	/**
	 * \struct	IconGridItem
	 * \brief	Represents an individual item within the IconGrid.
	 * 
	 * This class really only contains the name and image sheet coordinates of an
	 * item contained in the IconGrid. This could probably be done using a std::pair
	 * but this struct may grow in size later.
	 */
	struct IconGridItem
	{
		Point_2df		_imgSheetCoords;
		std::string		_name;
	};

	virtual void onMouseDown(MouseButton button, int x, int y);
	virtual void onMouseMotion(int x, int y, int dX, int dY);

	virtual void sizeChanged();

private:

	typedef std::vector<IconGridItem> IconItemList;

	void updateGrid();
	int translateCoordsToIndex(int x, int y);


	int					mHighlightIndex;		/**< Current highlight index. */
	int					mCurrentSelection;		/**< Currently selected item index. */
	
	int					mIconSize;				/**< Size of the icons. */
	int					mIconMargin;			/**< Spacing between icons and edges of the IconGrid. */

	bool				mShowTooltip;			/**< Flag indicating that we want a tooltip drawn near an icon when hovering over it. */

	Image				mIconSheet;				/**< Image containing the icons. */
	Point_2d			mGridSize;				/**< Dimensions of the grid that can be contained in the IconGrid with the current Icon Size and Icon Margin. */

	IconItemList		mIconItemList;			/**< List of items. */

	Callback			mCallback;				/**< Callback whenever a selection is made. */
};
