#pragma once

#include <libControls/Control.h>

#include "../Map/MapCoordinate.h"

#include <NAS2D/Resource/Image.h>


class Tile;
class TileMap;
class MapView;


class DetailMap : public Control
{
public:
	DetailMap(MapView& mapView, TileMap& tileMap, const std::string& tilesetPath);

	bool isMouseOverTile() const;

	MapCoordinate mouseTilePosition() const;
	Tile& mouseTile();

	void onMouseMove(NAS2D::Point<int> position);
	void resize(NAS2D::Vector<int>);

	void update() override;
	void draw() const override;

protected:
	void drawGrid() const;

private:
	MapView& mMapView;
	TileMap& mTileMap;
	const NAS2D::Image mTileset;
	const NAS2D::Image mMineBeacon;

	NAS2D::Point<int> mOriginPixelPosition; // Top pixel at top of diamond
	NAS2D::Point<int> mMouseTilePosition;
};
