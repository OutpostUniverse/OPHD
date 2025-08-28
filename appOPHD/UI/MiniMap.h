#pragma once

#include <libControls/Control.h>

#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>

#include <vector>
#include <string>


namespace NAS2D
{
	enum class MouseButton;
}

class Tile;
class TileMap;
class StructureManager;
class OreHaulRoutes;
class MapView;
class Robot;
class MapViewState;


class MiniMap : public Control
{
public:
	MiniMap(MapView& mapView, TileMap& tileMap, const StructureManager& structureManager, const std::vector<Robot*>& deployedRobots, const OreHaulRoutes& oreHaulRoutes, const std::string& mapName);

	bool heightMapVisible() const;
	void heightMapVisible(bool isVisible);

	void draw(NAS2D::Renderer& renderer) const override;

protected:
	friend MapViewState;
	void onActivate();
	void onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);
	void onSetView(NAS2D::Point<int> mousePixel);

private:
	MapView& mMapView;
	TileMap& mTileMap;
	const StructureManager& mStructureManager;
	const std::vector<Robot*>& mDeployedRobots;
	const OreHaulRoutes& mOreHaulRoutes;
	bool mIsHeightMapVisible;
	NAS2D::Image mBackgroundSatellite;
	NAS2D::Image mBackgroundHeightMap;
	const NAS2D::Image& mUiIcons;
	bool mLeftButtonDown{false};
};
