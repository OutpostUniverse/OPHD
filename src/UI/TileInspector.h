#pragma once

#include "UI.h"

#include "../Map/Tile.h"

class TileInspector: public Window
{
public:
	TileInspector();
	virtual ~TileInspector();

	void tile(Tile* t) { mTile = t; }

	virtual void update();

protected:
	void init();

private:
	TileInspector(const TileInspector&) = delete;
	TileInspector& operator=(const TileInspector&) = delete;

private:
	void btnCloseClicked();

private:
	Button		btnClose;
	Tile*		mTile = nullptr;
};
