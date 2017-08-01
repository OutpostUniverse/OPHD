#pragma once

#include "UI.h"

#include "../Map/Tile.h"

class TileInspector: public Window
{
public:

	TileInspector(Font& font);
	virtual ~TileInspector();

	virtual void update();

	void tile(Tile* t) { mTile = t; }

protected:

	virtual void init();

private:

	void btnCloseClicked();

	TileInspector();
	TileInspector(const TileInspector&);
	TileInspector& operator=(const TileInspector&);

	Button		btnClose;

	Font		mBold;

	Tile*		mTile;
};
