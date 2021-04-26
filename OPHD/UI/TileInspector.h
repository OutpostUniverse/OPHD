#pragma once

#include "Core/Window.h"
#include "Core/Button.h"

#include "../Map/Tile.h"


class TileInspector: public Window
{
public:
	TileInspector();
	~TileInspector() override;

	void tile(Tile* t) { mTile = t; }

	void update() override;

private:
	void onClose();

	Button btnClose;
	Tile* mTile = nullptr;
};
