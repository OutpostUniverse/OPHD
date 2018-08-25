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
	TileInspector() = delete;
	TileInspector(const TileInspector&) = delete;
	TileInspector& operator=(const TileInspector&) = delete;

private:
	void btnCloseClicked();

private:
	Button		btnClose;
	Font		mBold;
	Tile*		mTile;
};
