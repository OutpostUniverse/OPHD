#include "MiniMap.h"

#include "../CacheImage.h"
#include "../Map/OreHaulRoutes.h"
#include "../Map/Route.h"
#include "../Map/Tile.h"
#include "../Map/TileMap.h"
#include "../Map/MapView.h"
#include "../MapObjects/Robot.h"
#include "../MapObjects/Structures/CommTower.h"
#include "../StructureManager.h"

#include <libOPHD/MapObjects/OreDeposit.h>

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Renderer/Renderer.h>


namespace
{
	const std::string MapTerrainExtension = "_a.png";
	const std::string MapDisplayExtension = "_b.png";
}


MiniMap::MiniMap(MapView& mapView, TileMap& tileMap, const StructureManager& structureManager, const std::vector<Robot*>& deployedRobots, const OreHaulRoutes& oreHaulRoutes, const std::string& mapName) :
	mMapView{mapView},
	mTileMap{tileMap},
	mStructureManager{structureManager},
	mDeployedRobots{deployedRobots},
	mOreHaulRoutes{oreHaulRoutes},
	mIsOreHaulRoutesVisible{true},
	mIsHeightMapVisible{false},
	mBackgroundSatellite{mapName + MapDisplayExtension},
	mBackgroundHeightMap{mapName + MapTerrainExtension},
	mUiIcons{getImage("ui/icons.png")}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.activate().connect({this, &MiniMap::onActivate});
	eventHandler.mouseMotion().connect({this, &MiniMap::onMouseMove});
	eventHandler.mouseButtonDown().connect({this, &MiniMap::onMouseDown});
	eventHandler.mouseButtonUp().connect({this, &MiniMap::onMouseUp});
}


MiniMap::~MiniMap()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.activate().disconnect({this, &MiniMap::onActivate});
	eventHandler.mouseMotion().disconnect({this, &MiniMap::onMouseMove});
	eventHandler.mouseButtonDown().disconnect({this, &MiniMap::onMouseDown});
	eventHandler.mouseButtonUp().disconnect({this, &MiniMap::onMouseUp});
}


bool MiniMap::heightMapVisible() const
{
	return mIsHeightMapVisible;
}


void MiniMap::heightMapVisible(bool isVisible)
{
	mIsHeightMapVisible = isVisible;
}


/**
 * Draws the minimap and all icons/overlays for it.
 */
void MiniMap::draw(NAS2D::Renderer& renderer) const
{
	const auto miniMapFloatRect = mRect.to<float>();
	renderer.clipRect(miniMapFloatRect);

	renderer.drawImage((mIsHeightMapVisible ? mBackgroundHeightMap : mBackgroundSatellite), miniMapFloatRect.position);

	const auto miniMapOffset = mRect.position - NAS2D::Point{0, 0};
	for (const auto& ccPosition : mStructureManager.operationalCommandCenterPositions())
	{
		const auto ccOffsetPosition = ccPosition.xy + miniMapOffset;
		const auto ccCommRangeImageRect = NAS2D::Rectangle<int>{{166, 226}, {30, 30}};
		renderer.drawSubImage(mUiIcons, ccOffsetPosition - ccCommRangeImageRect.size / 2, ccCommRangeImageRect);
		renderer.drawBoxFilled(NAS2D::Rectangle{ccOffsetPosition - NAS2D::Vector{1, 1}, {3, 3}}, NAS2D::Color::White);
	}

	for (const auto* commTower : mStructureManager.getStructures<CommTower>())
	{
		if (commTower->operational())
		{
			const auto commTowerPosition = commTower->xyz().xy;
			const auto commTowerRangeImageRect = NAS2D::Rectangle<int>{{146, 236}, {20, 20}};
			renderer.drawSubImage(mUiIcons, commTowerPosition + miniMapOffset - commTowerRangeImageRect.size / 2, commTowerRangeImageRect);
		}
	}

	for (const auto* oreDeposit : mTileMap.oreDeposits())
	{
		if (!oreDeposit) { break; } // avoids potential race condition where an Ore Deposit is destroyed during an updated cycle.

		const auto& tile = mTileMap.getTile({oreDeposit->location(), 0});
		const auto mineBeaconStatusOffsetX = (!tile.hasStructure() || !tile.structure()->isMineFacility()) ? 0 :
			(!oreDeposit->isExhausted()) ? 8 : 16;
		const auto beaconImageRect = NAS2D::Rectangle<int>{{mineBeaconStatusOffsetX, 0}, {7, 7}};
		renderer.drawSubImage(mUiIcons, oreDeposit->location() + miniMapOffset - NAS2D::Vector{2, 2}, beaconImageRect);
	}

	if (mIsOreHaulRoutesVisible)
	{
		for (auto* tile : mOreHaulRoutes.getRouteOverlay())
		{
			const auto tilePosition = tile->xy();
			renderer.drawPoint(tilePosition + miniMapOffset, NAS2D::Color::Magenta);
		}
	}

	for (const auto* robot : mDeployedRobots)
	{
		const auto robotPosition = robot->tile().xy();
		renderer.drawPoint(robotPosition + miniMapOffset, NAS2D::Color::Cyan);
	}

	const auto& viewTileRect = mMapView.viewTileRect();
	renderer.drawBox(viewTileRect.translate(miniMapOffset + NAS2D::Vector{1, 1}), NAS2D::Color{0, 0, 0, 180});
	renderer.drawBox(viewTileRect.translate(miniMapOffset), NAS2D::Color::White);

	renderer.clipRectClear();
}


void MiniMap::onActivate(bool /*newActiveValue*/)
{
	mLeftButtonDown = false;
}


void MiniMap::onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> /*position*/)
{
	if (button == NAS2D::MouseButton::Left)
	{
		mLeftButtonDown = false;
	}
}


void MiniMap::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (button == NAS2D::MouseButton::Left)
	{
		mLeftButtonDown = true;
		if (mRect.contains(position))
		{
			onSetView(position);
		}
	}
}


void MiniMap::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (mLeftButtonDown)
	{
		if (mRect.contains(position))
		{
			onSetView(position);
		}
	}
}


void MiniMap::onSetView(NAS2D::Point<int> mousePixel)
{
	const auto position = NAS2D::Point{0, 0} + (mousePixel - mRect.position);
	mMapView.centerOn(position);
}
