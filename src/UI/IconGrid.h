#pragma once

#include "UI.h"

class IconGrid : public Control
{
public:

	typedef Gallant::Signal2<int, const IconGrid&> Callback;

	IconGrid();
	virtual ~IconGrid();

	virtual void update();

	void sheetPath(const std::string& _path);
	void iconSize(int _size);
	void iconMargin(int _margin);

	Callback& selectionChanged() { return mCallback; }

	void addItem(const std::string& name, int sheetIndex);

protected:

	struct IconGridItem
	{
		IconGridItem() : _name("Unnamed") {}
		~IconGridItem() {}

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

	int					mCurrentSelection;
	
	int					mIconSize;
	int					mIconMargin;

	int					mHighlightIndex;

	Point_2d			mGridSize;

	Image				mIconSheet;

	IconItemList		mIconItemList;

	Callback			mCallback;
};
