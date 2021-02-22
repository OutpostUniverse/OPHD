#include "TileMap.h"

#include "../Constants.h"
#include "../DirectionOffset.h"
#include "../Mine.h"
#include "../Things/Structures/Structure.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/Xml/XmlElement.h>

#include <algorithm>
#include <functional>
#include <random>
#include <array>


using namespace NAS2D;
using namespace NAS2D::Xml;


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
	{ Planet::Hostility::Low, {30, 50, 20} },
	{ Planet::Hostility::Medium, {45, 35, 20} },
	{ Planet::Hostility::High, {35, 20, 45} },
};


TileMap::TileMap(const std::string& mapPath, const std::string& tilesetPath, int maxDepth, int mineCount, Planet::Hostility hostility, bool shouldSetupMines) :
	mSizeInTiles{MAP_WIDTH, MAP_HEIGHT},
	mMaxDepth(maxDepth),
	mMapPath(mapPath),
	mTsetPath(tilesetPath),
	mTileset(tilesetPath),
	mMineBeacon("structures/mine_beacon.png")
{
	std::cout << "Loading '" << mapPath << "'... ";
	buildTerrainMap(mapPath);
	buildMouseMap();
	initMapDrawParams(Utility<Renderer>::get().size());

	if (shouldSetupMines) { setupMines(mineCount, hostility); }
	std::cout << "finished!" << std::endl;
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
	getTile(pt, 0).pushMine(nullptr);
}


bool TileMap::isValidPosition(NAS2D::Point<int> position, int level) const
{
	 return NAS2D::Rectangle{0, 0, mSizeInTiles.x, mSizeInTiles.y}.contains(position) && level >= 0 && level <= mMaxDepth;
}


Tile& TileMap::getTile(NAS2D::Point<int> position, int level)
{
	if (!isValidPosition(position, level))
	{
		throw std::runtime_error("Tile coordinates out of bounds: {" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(level) + "}");
	}
	const auto mapPosition = position.to<std::size_t>();
	return mTileMap[static_cast<std::size_t>(level)][mapPosition.y][mapPosition.x];
}


/**
 * Builds the terrain map.
 */
void TileMap::buildTerrainMap(const std::string& path)
{
	if (!Utility<Filesystem>::get().exists(path + MAP_TERRAIN_EXTENSION))
	{
		throw std::runtime_error("Given map file does not exist.");
	}

	const Image heightmap(path + MAP_TERRAIN_EXTENSION);

	const auto levelCount = static_cast<std::size_t>(mMaxDepth) + 1;
	mTileMap.resize(levelCount);
	for(std::size_t level = 0; level < levelCount; level++)
	{
		mTileMap[level].resize(static_cast<std::size_t>(mSizeInTiles.y));
		for (std::size_t i = 0; i < mTileMap[level].size(); i++)
		{
			mTileMap[level][i].resize(static_cast<std::size_t>(mSizeInTiles.x));
		}
	}

	/**
	 * Builds a terrain map based on the pixel color values in
	 * a maps height map.
	 * 
	 * Height maps by default are in grey-scale. This method assumes
	 * that all channels are the same value so it only looks at the red.
	 * Color values are divided by 50 to get a height value from 1 - 4.
	 */
	for(int depth = 0; depth <= mMaxDepth; depth++)
	{
		for(int row = 0; row < mSizeInTiles.y; row++)
		{
			for(int col = 0; col < mSizeInTiles.x; col++)
			{
				auto color = heightmap.pixelColor({col, row});
				auto& tile = getTile({col, row}, depth);
				tile = {{col, row}, depth, static_cast<TerrainType>(color.red / 50)};
				if (depth > 0) { tile.excavated(false); }
			}
		}
	}
}


/**
 * Creates mining locations around the map area.
 */
void TileMap::setupMines(int mineCount, Planet::Hostility hostility)
{
	if (hostility == Planet::Hostility::None) { return; }

	int yieldLow = mineCount * HostilityMineYieldTable.at(hostility)[0] / 100;
	int yieldMedium = mineCount * HostilityMineYieldTable.at(hostility)[1] / 100;
	int yieldHigh = mineCount * HostilityMineYieldTable.at(hostility)[2] / 100;

	// There will inevitably be cases where the total yield count will not match
	// the required mine count. In these cases just tack on the difference to the
	// low yield mines. Difficulty settings could shift this to other yields.
	int yieldTotal = yieldLow + yieldMedium + yieldHigh;
	if (yieldTotal < mineCount) { yieldLow += mineCount - yieldTotal; }

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distributionWidth(5, MAP_WIDTH - 5);
	std::uniform_int_distribution<int> distributionHeight(5, MAP_HEIGHT - 5);

	auto mwidth = std::bind(distributionWidth, std::ref(generator));
	auto mheight = std::bind(distributionHeight, std::ref(generator));
	auto randPoint = [&mwidth, &mheight]() { return NAS2D::Point{mwidth(), mheight()}; };

	auto generateMines = [&](int mineCountAtYield, MineProductionRate yield) {
		for (int i = 0; i < mineCountAtYield; ++i) {
			addMineSet(randPoint(), mMineLocations, yield);
		}
	};

	generateMines(yieldLow, MineProductionRate::Low);
	generateMines(yieldMedium, MineProductionRate::Medium);
	generateMines(yieldHigh, MineProductionRate::High);
}


void TileMap::addMineSet(NAS2D::Point<int> suggestedMineLocation, Point2dList& plist, MineProductionRate rate)
{
	// Mines should not be right next to each other
	// If mines are right next to each other, then overwrite the old location with the new mine parameters
	const auto mineLocation = findSurroundingMineLocation(suggestedMineLocation);

	auto& tile = getTile(mineLocation, 0);
	tile.pushMine(new Mine(rate));
	tile.index(TerrainType::Dozed);

	plist.push_back(mineLocation);
}


NAS2D::Point<int> TileMap::findSurroundingMineLocation(NAS2D::Point<int> centerPoint)
{
	if (getTile(centerPoint, 0).hasMine())
	{
		for (const auto& direction : DirectionScan323)
		{
			const auto point = centerPoint + direction;
			if (getTile(point, 0).hasMine()) { return point; }
		}
	}
	return centerPoint;
}


/**
 * Build a logic map for determining what tile the mouse is pointing at.
 */
void TileMap::buildMouseMap()
{
	// Sanity checks
	if (!Utility<Filesystem>::get().exists("ui/mouse_map.png"))
	{
		throw std::runtime_error("Unable to find the mouse map file.");
	}

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

	for(std::size_t row = 0; row < TILE_HEIGHT_ABSOLUTE; row++)
	{
		for(std::size_t col = 0; col < TILE_WIDTH; col++)
		{
			const Color c = mousemap.pixelColor({static_cast<int>(col), static_cast<int>(row)});
			if (c == NAS2D::Color::Yellow) { mMouseMap[row][col] = MouseMapRegion::MMR_BOTTOM_RIGHT; }
			else if (c == NAS2D::Color::Red) { mMouseMap[row][col] = MouseMapRegion::MMR_TOP_LEFT; }
			else if (c == NAS2D::Color::Blue) { mMouseMap[row][col] = MouseMapRegion::MMR_TOP_RIGHT; }
			else if (c == NAS2D::Color::Green) { mMouseMap[row][col] = MouseMapRegion::MMR_BOTTOM_LEFT; }
			else { mMouseMap[row][col] = MouseMapRegion::MMR_MIDDLE; }
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
	mMapPosition = NAS2D::Point{(size.x - TILE_WIDTH) / 2, (size.y - constants::BOTTOM_UI_HEIGHT - mEdgeLength * TILE_HEIGHT_ABSOLUTE) / 2};
	mMapBoundingBox = {(size.x - TILE_WIDTH * mEdgeLength) / 2, mMapPosition.y, TILE_WIDTH * mEdgeLength, TILE_HEIGHT_ABSOLUTE * mEdgeLength};
}


void TileMap::mapViewLocation(NAS2D::Point<int> point)
{
	mMapViewLocation = {
		std::clamp(point.x, 0, mSizeInTiles.x - mEdgeLength),
		std::clamp(point.y, 0, mSizeInTiles.y - mEdgeLength)
	};
}


/**
 * Convenience function to focus the TileMap's view on a specified tile.
 * 
 * \param	_t	Pointer to a Tile. Safe to pass nullptr.
 */
void TileMap::centerMapOnTile(Tile* _t)
{
	if (!_t) { return; }

	mapViewLocation(_t->position() - NAS2D::Vector{mEdgeLength, mEdgeLength} / 2);
	currentDepth(_t->depth());
}


/**
 * Returns true if the current tile highlight is actually within the visible diamond map.
 */
bool TileMap::tileHighlightVisible() const
{
	return isVisibleTile(mMapHighlight, mCurrentDepth);
}


void TileMap::draw()
{
	auto& renderer = Utility<Renderer>::get();

	int tsetOffset = mCurrentDepth > 0 ? TILE_HEIGHT : 0;
	const auto highlightOffset = mMapHighlight - mMapViewLocation;

	for (int row = 0; row < mEdgeLength; row++)
	{
		for (int col = 0; col < mEdgeLength; col++)
		{
			auto& tile = getTile(mMapViewLocation + NAS2D::Vector{col, row}, mCurrentDepth);

			if (tile.excavated())
			{
				const auto position = mMapPosition + NAS2D::Vector{(col - row) * TILE_HALF_WIDTH, (col + row) * TILE_HEIGHT_HALF_ABSOLUTE};
				const auto subImageRect = NAS2D::Rectangle{static_cast<int>(tile.index()) * TILE_WIDTH, tsetOffset, TILE_WIDTH, TILE_HEIGHT};
				const bool isTileHighlighted = NAS2D::Vector{col, row} == highlightOffset;

				renderer.drawSubImage(mTileset, position, subImageRect, overlayColor(tile.overlay(), isTileHighlighted));

				// Draw a beacon on an unoccupied tile with a mine
				if (tile.mine() != nullptr && !tile.thing())
				{
					uint8_t glow = static_cast<uint8_t>(120 + sin(mTimer.tick() / THROB_SPEED) * 57);
					const auto mineBeaconPosition = position + NAS2D::Vector{ 0, -64 };

					renderer.drawImage(mMineBeacon, mineBeaconPosition);
					renderer.drawSubImage(mMineBeacon, position + NAS2D::Vector{ 59, 15 }, NAS2D::Rectangle{ 59, 79, 10, 7 }, NAS2D::Color{ glow, glow, glow });
				}

				// Tell an occupying thing to update itself.
				if (tile.thing()) { tile.thing()->sprite().update(position); }
			}
		}
	}

	updateTileHighlight();
}


/**
 * Brute Force but works.
 */
void TileMap::updateTileHighlight()
{
	if (!mMapBoundingBox.contains(mMousePosition))
	{
		return;
	}

	/// In the case of even edge lengths, we need to adjust the mouse picking code a bit.
	const int evenEdgeLengthAdjust = (edgeLength() % 2 == 0) ? TILE_HALF_WIDTH : 0;
	const int offsetX = ((mMousePosition.x - mMapBoundingBox.x - evenEdgeLengthAdjust) / TILE_WIDTH);
	const int offsetY = ((mMousePosition.y - mMapBoundingBox.y) / TILE_HEIGHT_ABSOLUTE);
	const int transform = (mMapPosition.x - mMapBoundingBox.x) / TILE_WIDTH;
	NAS2D::Vector<int> highlightOffset = {-transform + offsetY + offsetX, transform + offsetY - offsetX};

	const int mmOffsetX = std::clamp((mMousePosition.x - mMapBoundingBox.x - evenEdgeLengthAdjust) % TILE_WIDTH, 0, TILE_WIDTH);
	const int mmOffsetY = (mMousePosition.y - mMapBoundingBox.y) % TILE_HEIGHT_ABSOLUTE;

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

	mMapHighlight = mMapViewLocation + highlightOffset;
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


static void serializeTile(XmlElement* _ti, int x, int y, int depth, TerrainType index)
{
	auto* t = new XmlElement("tile");
	t->attribute("x", x);
	t->attribute("y", y);
	t->attribute("depth", depth);
	t->attribute("index", static_cast<int>(index));

	_ti->linkEndChild(t);
}


void TileMap::serialize(NAS2D::Xml::XmlElement* element, const Planet::Attributes& planetAttributes)
{
	// ==========================================
	// MAP PROPERTIES
	// ==========================================
	XmlElement *properties = new XmlElement("properties");
	element->linkEndChild(properties);

	properties->attribute("sitemap", planetAttributes.mapImagePath);
	properties->attribute("tset", planetAttributes.tilesetPath);
	properties->attribute("diggingdepth", planetAttributes.maxDepth);
	// NAS2D only supports double for floating point conversions as of 26July2020
	properties->attribute("meansolardistance", static_cast<double>(planetAttributes.meanSolarDistance));
	// ==========================================
	// VIEW PARAMETERS
	// ==========================================
	XmlElement *viewparams = new XmlElement("view_parameters");
	element->linkEndChild(viewparams);

	viewparams->attribute("currentdepth", mCurrentDepth);
	viewparams->attribute("viewlocation_x", mMapViewLocation.x);
	viewparams->attribute("viewlocation_y", mMapViewLocation.y);

	// ==========================================
	// MINES
	// ==========================================
	XmlElement *mines = new XmlElement("mines");
	element->linkEndChild(mines);

	for (std::size_t i = 0; i < mMineLocations.size(); ++i)
	{
		XmlElement *mine = new XmlElement("mine");
		mine->attribute("x", mMineLocations[i].x);
		mine->attribute("y", mMineLocations[i].y);
		getTile(mMineLocations[i], TileMapLevel::LEVEL_SURFACE).mine()->serialize(mine);
		mines->linkEndChild(mine);
	}


	// ==========================================
	// TILES
	// ==========================================
	XmlElement *tiles = new XmlElement("tiles");
	element->linkEndChild(tiles);

	// We're only writing out tiles that don't have structures or robots in them that are
	// underground and excavated or surface and bulldozed.
	for (int depth = 0; depth <= maxDepth(); ++depth)
	{
		for (int y = 0; y < mSizeInTiles.y; ++y)
		{
			for (int x = 0; x < mSizeInTiles.x; ++x)
			{
				auto& tile = getTile({x, y}, depth);
				if (depth > 0 && tile.excavated() && tile.empty() && tile.mine() == nullptr)
				{
					serializeTile(tiles, x, y, depth, tile.index());
				}
				else if (tile.index() == TerrainType::Dozed && tile.empty() && tile.mine() == nullptr)
				{
					serializeTile(tiles, x, y, depth, tile.index());
				}
			}
		}
	}
}


void TileMap::deserialize(NAS2D::Xml::XmlElement* element)
{
	// VIEW PARAMETERS
	int view_x = 0, view_y = 0, view_depth = 0;
	auto* view_parameters = element->firstChildElement("view_parameters");
	auto* attribute = view_parameters->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "viewlocation_x") { attribute->queryIntValue(view_x); }
		else if (attribute->name() == "viewlocation_y") { attribute->queryIntValue(view_y); }
		else if (attribute->name() == "currentdepth") { attribute->queryIntValue(view_depth); }
		attribute = attribute->next();
	}

	mapViewLocation({view_x, view_y});
	currentDepth(view_depth);
	for (auto* mineElement = element->firstChildElement("mines")->firstChildElement("mine"); mineElement; mineElement = mineElement->nextSiblingElement())
	{
		int x = 0, y = 0;

		attribute = mineElement->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x") { attribute->queryIntValue(x); }
			else if (attribute->name() == "y") { attribute->queryIntValue(y); }
			attribute = attribute->next();
		}

		Mine* mine = new Mine();
		mine->deserialize(mineElement->toElement());

		auto& tile = getTile({x, y}, 0);
		tile.pushMine(mine);
		tile.index(TerrainType::Dozed);

		mMineLocations.push_back(Point{x, y});

		/// \fixme	Legacy code to assist in updating older versions of save games between 0.7.5 and 0.7.6. Remove in 0.8.0
		if (mine->depth() == 0 && mine->active()) { mine->increaseDepth(); }
	}

	// TILES AT INDEX 0 WITH NO THINGS
	for (auto* tileElement = element->firstChildElement("tiles")->firstChildElement("tile"); tileElement; tileElement = tileElement->nextSiblingElement())
	{
		int x = 0, y = 0, depth = 0, index = 0;

		attribute = tileElement->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x") { attribute->queryIntValue(x); }
			else if (attribute->name() == "y") { attribute->queryIntValue(y); }
			else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
			else if (attribute->name() == "index") { attribute->queryIntValue(index); }

			attribute = attribute->next();
		}

		auto& tile = getTile({x, y}, depth);
		tile.index(static_cast<TerrainType>(index));

		if (depth > 0) { tile.excavated(true); }
	}
}


Tile* TileMap::getVisibleTile(NAS2D::Point<int> position, int level)
{
	if (!isVisibleTile(position, level))
	{
		return nullptr;
	}

	return &getTile(position, level);
}


bool TileMap::isVisibleTile(NAS2D::Point<int> position, int z) const
{
	if (!NAS2D::Rectangle{mMapViewLocation.x, mMapViewLocation.y, mEdgeLength, mEdgeLength}.contains(position))
	{
		return false;
	}

	if (z != mCurrentDepth)
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
	return static_cast<Tile*>(stateStart)->distanceTo(static_cast<Tile*>(stateEnd));
}


void TileMap::AdjacentCost(void* state, std::vector<micropather::StateCost>* adjacent)
{
	auto& tile = *static_cast<Tile*>(state);
	const auto tilePosition = tile.position();

	for (const auto& offset : DirectionClockwise4)
	{
		const auto position = tilePosition + offset;
		if (!NAS2D::Rectangle{0, 0, mSizeInTiles.x, mSizeInTiles.y}.contains(position))
		{
			continue;
		}

		auto& adjacentTile = getTile(position, 0);
		float cost = constants::ROUTE_BASE_COST;

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
				cost = 0.5f;
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

		micropather::StateCost nodeCost = { &adjacentTile, cost };
		adjacent->push_back(nodeCost);
	}
}


void TileMap::pathStartAndEnd(void* start, void* end)
{
	mPathStartEndPair = std::make_pair(start, end);
}
