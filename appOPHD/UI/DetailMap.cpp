#include "DetailMap.h"

#include "../Map/Tile.h"
#include "../Map/TileMap.h"
#include "../Map/MapView.h"
#include "../MapObjects/MapObject.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/PointInRectangleRange.h>

#include <map>


namespace {
	const auto TileSize = NAS2D::Vector{128, 55};
	const auto TileDrawSize = NAS2D::Vector{128, 64};
	const auto TileDrawOffset = NAS2D::Vector{TileDrawSize.x / 2, TileDrawSize.y - TileSize.y};

	const std::map<Tile::Overlay, NAS2D::Color> OverlayColors =
	{
		{Tile::Overlay::None, NAS2D::Color::Normal},
		{Tile::Overlay::Communications, {125, 200, 255}},
		{Tile::Overlay::Connectedness, NAS2D::Color::Green},
		{Tile::Overlay::TruckingRoutes, NAS2D::Color::Orange},
		{Tile::Overlay::Police, NAS2D::Color::Red}
	};

	const std::map<Tile::Overlay, NAS2D::Color> OverlayHighlightColors =
	{
		{Tile::Overlay::None, NAS2D::Color{125, 200, 255}},
		{Tile::Overlay::Communications, {100, 180, 230}},
		{Tile::Overlay::Connectedness, NAS2D::Color{71, 224, 146}},
		{Tile::Overlay::TruckingRoutes, NAS2D::Color{125, 200, 255}},
		{Tile::Overlay::Police, NAS2D::Color{100, 180, 230}}
	};


	const NAS2D::Color& overlayColor(Tile::Overlay overlay, bool isHighlighted)
	{
		return (isHighlighted ? OverlayHighlightColors : OverlayColors).at(overlay);
	}


	uint8_t glow()
	{
		static NAS2D::Timer throbTimer;
		constexpr int throbCycleTime = 2000;
		constexpr int glowAmplitude = 120;
		constexpr int glowOffset = 120;

		int sawtooth = static_cast<int>(throbTimer.tick()) % throbCycleTime;
		int triangle = sawtooth < throbCycleTime / 2 ? sawtooth : (throbCycleTime - sawtooth);
		return static_cast<uint8_t>(triangle * glowAmplitude / (throbCycleTime / 2) + glowOffset);
	}


	NAS2D::Color glowColor()
	{
		uint8_t glowValue = glow();
		return NAS2D::Color{glowValue, glowValue, glowValue};
	}
}


DetailMap::DetailMap(MapView& mapView, TileMap& tileMap, const std::string& tilesetPath) :
	mMapView{mapView},
	mTileMap{tileMap},
	mTileset{tilesetPath},
	mMineBeacon{"structures/mine_beacon.png"}
{
	size(NAS2D::Utility<NAS2D::Renderer>::get().size());

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().connect({this, &DetailMap::onMouseMove});
}


DetailMap::~DetailMap()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().disconnect({this, &DetailMap::onMouseMove});
}


void DetailMap::onResize()
{
	const auto size = this->size() - NAS2D::Vector{0, TileDrawOffset.y};

	// Set up map draw position
	const auto sizeInTiles = size.skewInverseBy(TileSize);
	mMapView.viewSize(std::min(sizeInTiles.x, sizeInTiles.y));

	// Find top left corner of rectangle containing top tile of diamond
	mOriginPixelPosition = mRect.position + NAS2D::Vector{size.x / 2, TileDrawOffset.y + (size.y - mMapView.viewSize() * TileSize.y) / 2};
}


/**
 * Returns true if the current tile highlight is actually within the visible diamond map.
 */
bool DetailMap::isMouseOverTile() const
{
	return mMapView.isVisibleTile(mouseTilePosition());
}


MapCoordinate DetailMap::mouseTilePosition() const
{
	return {mMouseTilePosition, mMapView.currentDepth()};
}


Tile& DetailMap::mouseTile()
{
	if (!isMouseOverTile())
	{
		throw std::runtime_error("Mouse not over a tile");
	}
	return mTileMap.getTile(mouseTilePosition());
}


void DetailMap::update()
{
	for (const auto tilePosition : NAS2D::PointInRectangleRange{mMapView.viewTileRect()})
	{
		auto& tile = mTileMap.getTile({tilePosition, mMapView.currentDepth()});

		if (tile.mapObject())
		{
			tile.mapObject()->updateAnimation();
		}
	}
}


void DetailMap::draw(NAS2D::Renderer& renderer) const
{
	int tsetOffset = mMapView.currentDepth() > 0 ? TileDrawSize.y : 0;

	for (const auto tilePosition : NAS2D::PointInRectangleRange{mMapView.viewTileRect()})
	{
		auto& tile = mTileMap.getTile({tilePosition, mMapView.currentDepth()});

		if (tile.excavated())
		{
			const auto offset = tilePosition - mMapView.viewTileRect().position;
			const auto position = mOriginPixelPosition - TileDrawOffset + NAS2D::Vector{(offset.x - offset.y) * TileSize.x / 2, (offset.x + offset.y) * TileSize.y / 2};
			const auto subImageRect = NAS2D::Rectangle{{static_cast<int>(tile.index()) * TileDrawSize.x, tsetOffset}, TileDrawSize};
			const bool isTileHighlighted = tilePosition == mMouseTilePosition;

			renderer.drawSubImage(mTileset, position, subImageRect, overlayColor(tile.overlay(), isTileHighlighted));

			// Draw a beacon on an unoccupied tile with a mine
			if (mTileMap.hasOreDeposit(tile.xyz()) && !tile.mapObject())
			{
				constexpr NAS2D::Vector<int> beaconOffsetInTile{0, -64};
				constexpr NAS2D::Vector<int> beaconLightOffsetInTile{59, 15};
				constexpr NAS2D::Rectangle<int> beaconLightSubImageRect{{59, 79}, {10, 7}};
				renderer.drawImage(mMineBeacon, position + beaconOffsetInTile);
				renderer.drawSubImage(mMineBeacon, position + beaconLightOffsetInTile, beaconLightSubImageRect, glowColor());
			}

			if (tile.mapObject())
			{
				tile.mapObject()->draw(position);
			}
		}
	}
}


void DetailMap::drawGrid(NAS2D::Renderer& renderer) const
{
	const auto viewSize = mMapView.viewSize();
	const auto incrementY = NAS2D::Vector{-TileSize.x, TileSize.y};
	const auto leftEdge = mOriginPixelPosition + incrementY * viewSize / 2;
	const auto rightEdge = mOriginPixelPosition + TileSize * viewSize / 2;
	for (int index = 0; index <= viewSize; ++index)
	{
		const auto offsetX = TileSize * index / 2;
		const auto offsetY = incrementY * index / 2;
		renderer.drawLine(leftEdge + offsetX, mOriginPixelPosition + offsetX);
		renderer.drawLine(mOriginPixelPosition + offsetY, rightEdge + offsetY);
	}
}


void DetailMap::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	const auto pixelOffset = position - mOriginPixelPosition;
	const auto tileOffset = NAS2D::Vector{pixelOffset.x * TileSize.y + pixelOffset.y * TileSize.x, pixelOffset.y * TileSize.x - pixelOffset.x * TileSize.y} / (TileSize.x * TileSize.y);
	mMouseTilePosition = mMapView.viewTileRect().position + tileOffset;
}
