#include "TileMap.h"

#include "../Constants/Numbers.h"
#include "../Constants/UiConstants.h"
#include "../DirectionOffset.h"
#include "../Mine.h"
#include "../Things/Structures/Structure.h"
#include "../RandomNumberGenerator.h"

#include <NAS2D/Utility.h>
#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlElement.h>

#include <algorithm>
#include <functional>
#include <array>


using namespace NAS2D;


namespace {
	const std::string MAP_TERRAIN_EXTENSION = "_a.png";

	const int MAP_WIDTH = 300;
	const int MAP_HEIGHT = 150;

	const int TILE_WIDTH = 128;
	const int TILE_HEIGHT = 64;

	const int TILE_HALF_WIDTH = TILE_WIDTH / 2;

	const int TILE_HEIGHT_OFFSET = 9;
	const int TILE_HEIGHT_ABSOLUTE = TILE_HEIGHT - TILE_HEIGHT_OFFSET;
	const int TILE_HEIGHT_HALF_ABSOLUTE = TILE_HEIGHT_ABSOLUTE / 2;

	const double THROB_SPEED = 250.0; // Throb speed of mine beacon

	/** Array indicates percent of mines that should be of yields LOW, MED, HIGH */
	const std::map<Planet::Hostility, std::array<int, 3>> HostilityMineYieldTable =
	{
		{Planet::Hostility::Low, {30, 50, 20}},
		{Planet::Hostility::Medium, {45, 35, 20}},
		{Planet::Hostility::High, {35, 20, 45}},
	};


	const std::map<Tile::Overlay, NAS2D::Color> OverlayColorTable =
	{
		{Tile::Overlay::None, NAS2D::Color::Normal},
		{Tile::Overlay::Communications, {125, 200, 255}},
		{Tile::Overlay::Connectedness, NAS2D::Color::Green},
		{Tile::Overlay::TruckingRoutes, NAS2D::Color::Orange},
		{Tile::Overlay::Police, NAS2D::Color::Red}
	};

	const std::map<Tile::Overlay, NAS2D::Color> OverlayHighlightColorTable =
	{
		{Tile::Overlay::None, NAS2D::Color{125, 200, 255}},
		{Tile::Overlay::Communications, {100, 180, 230}},
		{Tile::Overlay::Connectedness, NAS2D::Color{71, 224, 146}},
		{Tile::Overlay::TruckingRoutes, NAS2D::Color{125, 200, 255}},
		{Tile::Overlay::Police, NAS2D::Color{100, 180, 230}}
	};


	const NAS2D::Color& overlayColor(Tile::Overlay overlay)
	{
		return OverlayColorTable.at(overlay);
	}


	const NAS2D::Color& overlayHighlightColor(Tile::Overlay overlay)
	{
		return OverlayHighlightColorTable.at(overlay);
	}


	const NAS2D::Color& overlayColor(Tile::Overlay overlay, bool isHighlighted)
	{
		if (isHighlighted)
		{
			return overlayHighlightColor(overlay);
		}

		return overlayColor(overlay);
	}


	std::vector<NAS2D::Point<int>> generateMineLocations(NAS2D::Vector<int> mapSize, std::size_t mineCount)
	{
		auto randPoint = [mapSize]() {
			return NAS2D::Point{
				randomNumber.generate<int>(5, mapSize.x - 5),
				randomNumber.generate<int>(5, mapSize.y - 5)
			};
		};

		std::vector<NAS2D::Point<int>> locations;
		locations.reserve(mineCount);

		// Some locations might not be acceptable, so try up to twice as many locations
		// A high density of mines could result in many rejected locations
		// Don't try indefinitely to avoid possibility of infinite loop
		std::vector<bool> usedLocations(mapSize.x * mapSize.y);
		for (std::size_t i = 0; (locations.size() < mineCount) && (i < mineCount * 2); ++i)
		{
			// Generate a location and check surroundings for minimum spacing
			const auto point = randPoint();
			if (!usedLocations[point.x + mapSize.x * point.y])
			{
				locations.push_back(point);
				for (const auto& offset : DirectionScan3x3)
				{
					const auto usedPoint = point + offset;
					usedLocations[usedPoint.x + mapSize.x * usedPoint.y] = true;
				}
			}
		}

		return locations;
	}


	void placeMines(TileMap& tileMap, Planet::Hostility hostility, const std::vector<NAS2D::Point<int>>& locations)
	{
		const auto& mineYields = HostilityMineYieldTable.at(hostility);
		const auto total = std::accumulate(mineYields.begin(), mineYields.end(), 0);

		const auto randYield = [mineYields, total]() {
			const auto randValue = randomNumber.generate<int>(1, total);
			return (randValue <= mineYields[0]) ? MineProductionRate::Low :
				(randValue <= mineYields[0] + mineYields[1]) ? MineProductionRate::Medium :
				MineProductionRate::High;
		};

		for (const auto& location : locations)
		{
			auto& tile = tileMap.getTile({location, 0});
			tile.pushMine(new Mine(randYield()));
			tile.index(TerrainType::Dozed);
		}
	}
}


TileMap::TileMap(const std::string& mapPath, const std::string& tilesetPath, int maxDepth, int mineCount, Planet::Hostility hostility, bool shouldSetupMines) :
	mSizeInTiles{MAP_WIDTH, MAP_HEIGHT},
	mMaxDepth(maxDepth),
	mMapPath(mapPath),
	mTsetPath(tilesetPath),
	mTileset(tilesetPath),
	mMineBeacon("structures/mine_beacon.png")
{
	buildTerrainMap(mapPath);
	buildMouseMap();
	initMapDrawParams(Utility<Renderer>::get().size());

	if (shouldSetupMines)
	{
		mMineLocations = generateMineLocations(mSizeInTiles, mineCount);
		placeMines(*this, hostility, mMineLocations);
	}
}


/**
 * Removes a mine location from the tilemap.
 * 
 * \note	Does no sanity checking, assumes that the point provided
 *			corresponds to a valid location.
 */
void TileMap::removeMineLocation(const NAS2D::Point<int>& pt)
{
	mMineLocations.erase(find(mMineLocations.begin(), mMineLocations.end(), pt));
	getTile({pt, 0}).pushMine(nullptr);
}


bool TileMap::isValidPosition(const MapCoordinate& position) const
{
	return NAS2D::Rectangle{0, 0, mSizeInTiles.x, mSizeInTiles.y}.contains(position.xy) && position.z >= 0 && position.z <= mMaxDepth;
}


const Tile& TileMap::getTile(const MapCoordinate& position) const
{
	if (!isValidPosition(position))
	{
		throw std::runtime_error("Tile coordinates out of bounds: {" + std::to_string(position.xy.x) + ", " + std::to_string(position.xy.y) + ", " + std::to_string(position.z) + "}");
	}
	const auto mapPosition = position.xy.to<std::size_t>();
	const auto level = static_cast<std::size_t>(position.z);
	return mTileMap[((level * mSizeInTiles.y) + mapPosition.y) * mSizeInTiles.x + mapPosition.x];
}


Tile& TileMap::getTile(const MapCoordinate& position)
{
	const auto& constThis = *this;
	return const_cast<Tile&>(constThis.getTile(position));
}


/**
 * Builds the terrain map.
 */
void TileMap::buildTerrainMap(const std::string& path)
{
	const Image heightmap(path + MAP_TERRAIN_EXTENSION);

	const auto levelCount = static_cast<std::size_t>(mMaxDepth) + 1;
	mTileMap.resize(mSizeInTiles.x * mSizeInTiles.y * levelCount);

	/**
	 * Builds a terrain map based on the pixel color values in
	 * a maps height map.
	 * 
	 * Height maps by default are in grey-scale. This method assumes
	 * that all channels are the same value so it only looks at the red.
	 * Color values are divided by 50 to get a height value from 1 - 4.
	 */
	for (int depth = 0; depth <= mMaxDepth; depth++)
	{
		for (int row = 0; row < mSizeInTiles.y; row++)
		{
			for (int col = 0; col < mSizeInTiles.x; col++)
			{
				auto color = heightmap.pixelColor({col, row});
				auto& tile = getTile({{col, row}, depth});
				tile = {{{col, row}, depth}, static_cast<TerrainType>(color.red / 50)};
				if (depth > 0) { tile.excavated(false); }
			}
		}
	}
}


/**
 * Build a logic map for determining what tile the mouse is pointing at.
 */
void TileMap::buildMouseMap()
{
	const Image mousemap("ui/mouse_map.png");

	// More sanity checks (mousemap should match dimensions of tile)
	if (mousemap.size() != Vector{TILE_WIDTH, TILE_HEIGHT_ABSOLUTE})
	{
		throw std::runtime_error("Mouse map is the wrong dimensions.");
	}

	mMouseMap.resize(TILE_HEIGHT_ABSOLUTE);
	for (std::size_t i = 0; i < mMouseMap.size(); i++)
	{
		mMouseMap[i].resize(TILE_WIDTH);
	}

	for (std::size_t row = 0; row < TILE_HEIGHT_ABSOLUTE; row++)
	{
		for (std::size_t col = 0; col < TILE_WIDTH; col++)
		{
			const Color c = mousemap.pixelColor({static_cast<int>(col), static_cast<int>(row)});
			if (c == NAS2D::Color::Yellow)
			{
				mMouseMap[row][col] = MouseMapRegion::MMR_BOTTOM_RIGHT;
			}
			else if (c == NAS2D::Color::Red)
			{
				mMouseMap[row][col] = MouseMapRegion::MMR_TOP_LEFT;
			}
			else if (c == NAS2D::Color::Blue)
			{
				mMouseMap[row][col] = MouseMapRegion::MMR_TOP_RIGHT;
			}
			else if (c == NAS2D::Color::Green)
			{
				mMouseMap[row][col] = MouseMapRegion::MMR_BOTTOM_LEFT;
			}
			else
			{
				mMouseMap[row][col] = MouseMapRegion::MMR_MIDDLE;
			}
		}
	}
}


/**
 * Sets up position and drawing parememters for the tile map.
 */
void TileMap::initMapDrawParams(NAS2D::Vector<int> size)
{
	// Set up map draw position
	const auto lengthX = size.x / TILE_WIDTH;
	const auto lengthY = size.y / TILE_HEIGHT_ABSOLUTE;
	mEdgeLength = std::max(3, std::min(lengthX, lengthY));

	// Find top left corner of rectangle containing top tile of diamond
	mOriginPixelPosition = NAS2D::Point{(size.x - TILE_WIDTH) / 2, (size.y - constants::BottomUiHeight - mEdgeLength * TILE_HEIGHT_ABSOLUTE) / 2};
	mMapBoundingBox = {(size.x - TILE_WIDTH * mEdgeLength) / 2, mOriginPixelPosition.y, TILE_WIDTH * mEdgeLength, TILE_HEIGHT_ABSOLUTE * mEdgeLength};
}


void TileMap::mapViewLocation(NAS2D::Point<int> point)
{
	mOriginTilePosition = {
		std::clamp(point.x, 0, mSizeInTiles.x - mEdgeLength),
		std::clamp(point.y, 0, mSizeInTiles.y - mEdgeLength)
	};
}


void TileMap::mapViewLocation(const MapCoordinate& position)
{
	mapViewLocation(position.xy);
	currentDepth(position.z);
}


void TileMap::centerOn(NAS2D::Point<int> point)
{
	centerOn({point, mMouseTilePosition.z});
}


void TileMap::centerOn(const MapCoordinate& position)
{
	mapViewLocation({position.xy - NAS2D::Vector{mEdgeLength, mEdgeLength} / 2, position.z});
}


void TileMap::moveView(Direction direction)
{
	mapViewLocation({
		mOriginTilePosition + directionEnumToOffset(direction),
		mMouseTilePosition.z + directionEnumToVerticalOffset(direction)
	});
}


void TileMap::currentDepth(int i)
{
	mMouseTilePosition.z = std::clamp(i, 0, mMaxDepth);
}


/**
 * Returns true if the current tile highlight is actually within the visible diamond map.
 */
bool TileMap::tileHighlightVisible() const
{
	return isVisibleTile(mouseTilePosition());
}


void TileMap::update()
{
	for (int row = 0; row < mEdgeLength; row++)
	{
		for (int col = 0; col < mEdgeLength; col++)
		{
			auto& tile = getTile({mOriginTilePosition + NAS2D::Vector{col, row}, mMouseTilePosition.z});

			if (tile.excavated())
			{
				// Tell an occupying thing to update itself.
				if (tile.thing())
				{
					auto& sprite = tile.thing()->sprite();
					sprite.update();
				}
			}
		}
	}

	updateTileHighlight();
}


void TileMap::draw() const
{
	auto& renderer = Utility<Renderer>::get();

	int tsetOffset = mMouseTilePosition.z > 0 ? TILE_HEIGHT : 0;
	const auto highlightOffset = mMouseTilePosition.xy - mOriginTilePosition;

	for (int row = 0; row < mEdgeLength; row++)
	{
		for (int col = 0; col < mEdgeLength; col++)
		{
			auto& tile = getTile({mOriginTilePosition + NAS2D::Vector{col, row}, mMouseTilePosition.z});

			if (tile.excavated())
			{
				const auto position = mOriginPixelPosition + NAS2D::Vector{(col - row) * TILE_HALF_WIDTH, (col + row) * TILE_HEIGHT_HALF_ABSOLUTE};
				const auto subImageRect = NAS2D::Rectangle{static_cast<int>(tile.index()) * TILE_WIDTH, tsetOffset, TILE_WIDTH, TILE_HEIGHT};
				const bool isTileHighlighted = NAS2D::Vector{col, row} == highlightOffset;

				renderer.drawSubImage(mTileset, position, subImageRect, overlayColor(tile.overlay(), isTileHighlighted));

				// Draw a beacon on an unoccupied tile with a mine
				if (tile.mine() != nullptr && !tile.thing())
				{
					uint8_t glow = static_cast<uint8_t>(120 + sin(mTimer.tick() / THROB_SPEED) * 57);
					const auto mineBeaconPosition = position + NAS2D::Vector{0, -64};

					renderer.drawImage(mMineBeacon, mineBeaconPosition);
					renderer.drawSubImage(mMineBeacon, position + NAS2D::Vector{59, 15}, NAS2D::Rectangle{59, 79, 10, 7}, NAS2D::Color{glow, glow, glow});
				}

				// Tell an occupying thing to update itself.
				if (tile.thing())
				{
					auto& sprite = tile.thing()->sprite();
					sprite.draw(position);
				}
			}
		}
	}
}


/**
 * Brute Force but works.
 */
void TileMap::updateTileHighlight()
{
	if (!mMapBoundingBox.contains(mMousePixelPosition))
	{
		return;
	}

	/// In the case of even edge lengths, we need to adjust the mouse picking code a bit.
	const int evenEdgeLengthAdjust = (edgeLength() % 2 == 0) ? TILE_HALF_WIDTH : 0;
	const int offsetX = ((mMousePixelPosition.x - mMapBoundingBox.x - evenEdgeLengthAdjust) / TILE_WIDTH);
	const int offsetY = ((mMousePixelPosition.y - mMapBoundingBox.y) / TILE_HEIGHT_ABSOLUTE);
	const int transform = (mOriginPixelPosition.x - mMapBoundingBox.x) / TILE_WIDTH;
	NAS2D::Vector<int> highlightOffset = {-transform + offsetY + offsetX, transform + offsetY - offsetX};

	const int mmOffsetX = std::clamp((mMousePixelPosition.x - mMapBoundingBox.x - evenEdgeLengthAdjust) % TILE_WIDTH, 0, TILE_WIDTH);
	const int mmOffsetY = (mMousePixelPosition.y - mMapBoundingBox.y) % TILE_HEIGHT_ABSOLUTE;

	switch (getMouseMapRegion(mmOffsetX, mmOffsetY))
	{
	case MouseMapRegion::MMR_TOP_RIGHT:
		--highlightOffset.y;
		break;

	case MouseMapRegion::MMR_TOP_LEFT:
		--highlightOffset.x;
		break;

	case MouseMapRegion::MMR_BOTTOM_RIGHT:
		++highlightOffset.x;
		break;

	case MouseMapRegion::MMR_BOTTOM_LEFT:
		++highlightOffset.y;
		break;

	default:
		break;
	}

	mMouseTilePosition.xy = mOriginTilePosition + highlightOffset;
}


/**
 * Takes a point and determines where in the mouse map that point lies.
 *
 * \note	Assumes coords are normalized to the boundaries of a tile.
 */
TileMap::MouseMapRegion TileMap::getMouseMapRegion(int x, int y)
{
	const auto mapPosition = NAS2D::Point{x, y}.to<std::size_t>();
	return mMouseMap[mapPosition.y][mapPosition.x];
}


void TileMap::serialize(NAS2D::Xml::XmlElement* element)
{
	// ==========================================
	// VIEW PARAMETERS
	// ==========================================
	element->linkEndChild(NAS2D::dictionaryToAttributes(
		"view_parameters",
		{{
			{"currentdepth", mMouseTilePosition.z},
			{"viewlocation_x", mOriginTilePosition.x},
			{"viewlocation_y", mOriginTilePosition.y},
		}}
	));

	// ==========================================
	// MINES
	// ==========================================
	auto* mines = new NAS2D::Xml::XmlElement("mines");
	element->linkEndChild(mines);

	for (const auto& location : mMineLocations)
	{
		auto& mine = *getTile({location, TileMapLevel::LEVEL_SURFACE}).mine();
		mines->linkEndChild(mine.serialize(location));
	}


	// ==========================================
	// TILES
	// ==========================================
	auto* tiles = new NAS2D::Xml::XmlElement("tiles");
	element->linkEndChild(tiles);

	// We're only writing out tiles that don't have structures or robots in them that are
	// underground and excavated or surface and bulldozed.
	for (int depth = 0; depth <= maxDepth(); ++depth)
	{
		for (int y = 0; y < mSizeInTiles.y; ++y)
		{
			for (int x = 0; x < mSizeInTiles.x; ++x)
			{
				auto& tile = getTile({{x, y}, depth});
				if (
					((depth > 0 && tile.excavated()) || (tile.index() == TerrainType::Dozed)) &&
					(tile.empty() && tile.mine() == nullptr)
				)
				{
					tiles->linkEndChild(
						NAS2D::dictionaryToAttributes(
							"tile",
							{{
								{"x", x},
								{"y", y},
								{"depth", depth},
								{"index", static_cast<int>(tile.index())},
							}}
						)
					);
				}
			}
		}
	}
}


void TileMap::deserialize(NAS2D::Xml::XmlElement* element)
{
	// VIEW PARAMETERS
	auto* view_parameters = element->firstChildElement("view_parameters");
	const auto dictionary = NAS2D::attributesToDictionary(*view_parameters);

	const auto view_x = dictionary.get<int>("viewlocation_x");
	const auto view_y = dictionary.get<int>("viewlocation_y");
	const auto view_depth = dictionary.get<int>("currentdepth");

	mapViewLocation({{view_x, view_y}, view_depth});

	for (auto* mineElement = element->firstChildElement("mines")->firstChildElement("mine"); mineElement; mineElement = mineElement->nextSiblingElement())
	{
		const auto mineDictionary = NAS2D::attributesToDictionary(*mineElement);

		const auto x = mineDictionary.get<int>("x");
		const auto y = mineDictionary.get<int>("y");

		Mine* mine = new Mine();
		mine->deserialize(mineElement);

		auto& tile = getTile({{x, y}, 0});
		tile.pushMine(mine);
		tile.index(TerrainType::Dozed);

		mMineLocations.push_back(Point{x, y});

		/// \fixme	Legacy code to assist in updating older versions of save games between 0.7.5 and 0.7.6. Remove in 0.8.0
		if (mine->depth() == 0 && mine->active()) { mine->increaseDepth(); }
	}

	// TILES AT INDEX 0 WITH NO THINGS
	for (auto* tileElement = element->firstChildElement("tiles")->firstChildElement("tile"); tileElement; tileElement = tileElement->nextSiblingElement())
	{
		const auto tileDictionary = NAS2D::attributesToDictionary(*tileElement);

		const auto x = tileDictionary.get<int>("x");
		const auto y = tileDictionary.get<int>("y");
		const auto depth = tileDictionary.get<int>("depth");
		const auto index = tileDictionary.get<int>("index");

		auto& tile = getTile({{x, y}, depth});
		tile.index(static_cast<TerrainType>(index));

		if (depth > 0) { tile.excavated(true); }
	}
}


Tile* TileMap::getVisibleTile(const MapCoordinate& position)
{
	if (!isVisibleTile(position))
	{
		return nullptr;
	}

	return &getTile(position);
}


bool TileMap::isVisibleTile(const MapCoordinate& position) const
{
	if (!NAS2D::Rectangle{mOriginTilePosition.x, mOriginTilePosition.y, mEdgeLength, mEdgeLength}.contains(position.xy))
	{
		return false;
	}

	if (position.z != mMouseTilePosition.z)
	{
		return false;
	}

	return true;
}


/**
 * Implements MicroPather interface.
 * 
 * \warning	Assumes stateStart and stateEnd are never nullptr.
 */
float TileMap::LeastCostEstimate(void* stateStart, void* stateEnd)
{
	return sqrtf(static_cast<float>((static_cast<Tile*>(stateEnd)->xy() - static_cast<Tile*>(stateStart)->xy()).lengthSquared()));
}


void TileMap::AdjacentCost(void* state, std::vector<micropather::StateCost>* adjacent)
{
	auto& tile = *static_cast<Tile*>(state);
	const auto tilePosition = tile.xy();

	for (const auto& offset : DirectionClockwise4)
	{
		const auto position = tilePosition + offset;
		if (!NAS2D::Rectangle{0, 0, mSizeInTiles.x, mSizeInTiles.y}.contains(position))
		{
			continue;
		}

		auto& adjacentTile = getTile({position, 0});
		float cost = constants::RouteBaseCost;

		if (adjacentTile.index() == TerrainType::Impassable)
		{
			cost = FLT_MAX;
		}
		else if (!adjacentTile.empty())
		{
			if (&adjacentTile == mPathStartEndPair.first || &adjacentTile == mPathStartEndPair.second)
			{
				cost *= static_cast<float>(adjacentTile.index()) + 1.0f;
			}
			else if (adjacentTile.thingIsStructure() && adjacentTile.structure()->structureId() == StructureID::SID_ROAD)
			{
				Structure& road = *adjacentTile.structure();

				if (road.state() != StructureState::Operational)
				{
					cost *= static_cast<float>(TerrainType::Difficult) + 1.0f;
				}
				else if (road.integrity() < constants::RoadIntegrityChange)
				{
					cost = 0.75f;
				}
				else
				{
					cost = 0.5f;
				}		
			}
			else
			{
				cost = FLT_MAX;
			}
		}
		else
		{
			cost *= static_cast<float>(adjacentTile.index()) + 1.0f;
		}

		micropather::StateCost nodeCost = {&adjacentTile, cost};
		adjacent->push_back(nodeCost);
	}
}


void TileMap::pathStartAndEnd(void* start, void* end)
{
	mPathStartEndPair = std::make_pair(start, end);
}
