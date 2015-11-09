#pragma once

#include "UI.h"

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

	void clearSelection();

	Callback& selectionChanged() { return mCallback; }

	virtual void hide();

protected:

	struct IconGridItem
	{
		Point_2df		_imgSheetCoords;
		std::string		_name;
	};

	virtual void onMouseDown(MouseButton button, int x, int y);
	virtual void onMouseMotion(int x, int y, int dX, int dY);

	virtual void positionChanged(float dX, float dY);
	virtual void sizeChanged();

private:

	typedef std::vector<IconGridItem> IconItemList;

	void updateGrid();

	int translateCoordsToIndex(int x, int y);


	int					mHighlightIndex;		/**< Current highlight index. */
	int					mCurrentSelection;		/**< Currently selected item index. */
	
	int					mIconSize;				/**< Size of the icons. */
	int					mIconMargin;			/**< Spacing between icons and edges of the IconGrid. */

	Image				mIconSheet;				/**< Image containing the icons. */
	Point_2d			mGridSize;				/**< Dimensions of the grid that can be contained in the IconGrid with the current Icon Size and Icon Margin. */

	IconItemList		mIconItemList;			/**< List of items. */

	Callback			mCallback;				/**< Callback whenever a selection is made. */
};
