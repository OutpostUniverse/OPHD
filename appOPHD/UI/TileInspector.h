#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>

#include "../Map/Tile.h"


class TileInspector: public Window
{
public:
	TileInspector();

	void tile(Tile& tile) { mTile = &tile; }

	void drawClientArea(NAS2D::Renderer& renderer) const override;

protected:
	void onVisibilityChange(bool visible) override;
	void onClose();

private:
	Button btnClose;
	Tile* mTile = nullptr;
};
