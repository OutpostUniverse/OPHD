#ifndef __TILE_INSPECTOR__
#define __TILE_INSPECTOR__

#include "UI.h"

#include "../Tile.h"

class TileInspector: public UIContainer
{
public:

	TileInspector(Font& font);
	virtual ~TileInspector();

	virtual void update();

	void tile(Tile* t) { mTile = t; }

protected:

	virtual void init();

	virtual void onMouseDown(MouseButton button, int x, int y);
	virtual void onMouseUp(MouseButton button, int x, int y);

private:

	void btnCloseClicked();

	TileInspector();
	TileInspector(const TileInspector&);
	TileInspector& operator=(const TileInspector&);

	Button		btnClose;

	Tile*		mTile;

};


#endif