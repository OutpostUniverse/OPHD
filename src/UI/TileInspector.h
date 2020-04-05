#pragma once

#include "UI.h"

#include "../Map/Tile.h"

class TileInspector: public Window
{
public:
	TileInspector();
	~TileInspector() override;

	void tile(Tile* t) { mTile = t; }

	void update() override;

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
