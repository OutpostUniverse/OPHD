// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TileMap.h"

#include "../Constants.h"
#include "../DirectionOffset.h"

#include <algorithm>
#include <functional>
#include <random>
#include <tuple>

using namespace NAS2D;
using namespace NAS2D::Xml;

#pragma warning(disable: 4244)

// ===============================================================================
// = CONSTANTS
// ===============================================================================
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


/** Tuple indicates percent of mines that should be of yields LOW, MED, HIGH */
std::map<constants::PlanetHostility, std::tuple<float, float, float>> HostilityMineYieldTable =
{
	{ constants::PlanetHostility::HOSTILITY_LOW, {0.30f, 0.50f, 0.20f} },
	{ constants::PlanetHostility::HOSTILITY_MEDIUM, {0.45f, 0.35f, 0.20f} },
	{ constants::PlanetHostility::HOSTILITY_HIGH, {0.35f, 0.20f, 0.45f} },
};


// ===============================================================================
// = LOCAL VARIABLES
// ===============================================================================
Point<int> TRANSFORM; /**< Used to adjust mouse and screen spaces based on position of the map field. */


// ===============================================================================
// = STATIC/LOCAL FUNCTIONS
// ===============================================================================
using TileArray = std::vector<std::vector<std::vector<Tile> > >;
static Point<int> findSurroundingMineLocation(Point<int> centerPoint, TileArray& tileArray)
{
	if (tileArray[0][centerPoint.y()][centerPoint.x()].hasMine())
	{
		for (const auto& direction : DirectionScan323)
		{
			const auto point = centerPoint + direction;
			if (tileArray[0][point.y()][point.x()].hasMine()) { return point; }
		}
	}
	return centerPoint;
}


static void addMineSet(Point<int> suggestedMineLocation, Point2dList& plist, TileArray& tileArray, MineProductionRate rate)
{
	// Mines should not be right next to each other
	// If mines are right next to each other, then overwrite the old location with the new mine parameters
	const auto mineLocation = findSurroundingMineLocation(suggestedMineLocation, tileArray);

	tileArray[0][mineLocation.y()][mineLocation.x()].pushMine(new Mine(rate));
	tileArray[0][mineLocation.y()][mineLocation.x()].index(TerrainType::TERRAIN_DOZED);

	plist.push_back(mineLocation);
}



// ===============================================================================
// = CLASS/PUBLIC FUNCTIONS
// ===============================================================================
/**
 * C'tor
 */
TileMap::TileMap(const std::string& mapPath, const std::string& tilesetPath, int maxDepth, int mineCount, constants::PlanetHostility hostility, bool shouldSetupMines) :
	mWidth(MAP_WIDTH),
	mHeight(MAP_HEIGHT),
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


TileMap::~TileMap()
{}


/**
 * Removes a mine location from the tilemap.
 * 
 * \note	Does no sanity checking, assumes that the point provided
 *			corresponds to a valid location.
 */
void TileMap::removeMineLocation(const NAS2D::Point<int>& pt)
{
	mMineLocations.erase(find(mMineLocations.begin(), mMineLocations.end(), pt));
	getTile(pt, 0)->pushMine(nullptr);
}


Tile* TileMap::getTile(NAS2D::Point<int> position, int level)
{
	if (NAS2D::Rectangle{0, 0, mWidth, mHeight}.contains(position) && level >= 0 && level <= mMaxDepth)
	{
		return &mTileMap[level][position.y()][position.x()];
	}

	return nullptr;
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

	Image heightmap(path + MAP_TERRAIN_EXTENSION);

	mTileMap.resize(static_cast<std::size_t>(mMaxDepth) + 1);
	for(int level = 0; level <= mMaxDepth; level++)
	{
		mTileMap[level].resize(height());
		for (std::size_t i = 0; i < mTileMap[level].size(); i++)
		{
			mTileMap[level][i].resize(width());
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
		for(int row = 0; row < height(); row++)
		{
			for(int col = 0; col < width(); col++)
			{
				Color c = heightmap.pixelColor(col, row);
				Tile& t = mTileMap[depth][row][col];
				t.init(col, row, depth, c.red / 50);
				if (depth > 0) { t.excavated(false); }
			}
		}
	}
}


/**
 * Creates mining locations around the map area.
 */
void TileMap::setupMines(int mineCount, constants::PlanetHostility hostility)
{
	if (hostility == constants::PlanetHostility::HOSTILITY_NONE) { return; }

	int yield_low = mineCount * std::get<0>(HostilityMineYieldTable[hostility]);
	int yield_medium = mineCount * std::get<1>(HostilityMineYieldTable[hostility]);
	int yield_high = mineCount * std::get<2>(HostilityMineYieldTable[hostility]);

	// There will inevitably be cases where the total yield count will not match
	// the required mine count. In these cases just tack on the difference to the
	// low yield mines. Difficulty settings could shift this to other yields.
	int yield_total = yield_low + yield_medium + yield_high;
	if (yield_total < mineCount) { yield_low += mineCount - yield_total; }

	// no check for overflows here because of the nature of division operations
	// on int types. Yield totals should only ever equate to mineCount or less
	// than mineCount.

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> map_width(5, MAP_WIDTH - 5);
	std::uniform_int_distribution<int> map_height(5, MAP_HEIGHT - 5);

	auto mwidth = std::bind(map_width, std::ref(generator));
	auto mheight = std::bind(map_height, std::ref(generator));

	// \fixme Inelegant solution but may not be worth refactoring out into its own function.
	for (int i = 0; i < yield_low; ++i)
	{
		Point<int> pt{std::clamp(mwidth(), 4, mWidth - 8), std::clamp(mheight(), 4, mWidth - 8)};
		addMineSet(pt, mMineLocations, mTileMap, MineProductionRate::PRODUCTION_RATE_LOW);
	}

	for (int i = 0; i < yield_medium; ++i)
	{
		Point<int> pt{std::clamp(mwidth(), 4, mWidth - 8), std::clamp(mheight(), 4, mWidth - 8)};
		addMineSet(pt, mMineLocations, mTileMap, MineProductionRate::PRODUCTION_RATE_MEDIUM);
	}

	for (int i = 0; i < yield_high; ++i)
	{
		Point<int> pt{std::clamp(mwidth(), 4, mWidth - 8), std::clamp(mheight(), 4, mWidth - 8)};
		addMineSet(pt, mMineLocations, mTileMap, MineProductionRate::PRODUCTION_RATE_HIGH);
	}

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

	Image mousemap("ui/mouse_map.png");

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
			const Color c = mousemap.pixelColor(static_cast<int>(col), static_cast<int>(row));
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
	mEdgeLength = size.x / TILE_WIDTH;

	mMapPosition = {static_cast<float>(size.x / 2 - (TILE_WIDTH / 2)), ((size.y - constants::BOTTOM_UI_HEIGHT) / 2) - ((static_cast<float>(mEdgeLength) / 2) * TILE_HEIGHT_ABSOLUTE)};
	mMapBoundingBox = {(size.x / 2) - ((TILE_WIDTH * mEdgeLength) / 2), static_cast<int>(mMapPosition.y()), TILE_WIDTH * mEdgeLength, TILE_HEIGHT_ABSOLUTE * mEdgeLength};

	int transform = (mMapPosition.x() - mMapBoundingBox.x()) / TILE_WIDTH;
	TRANSFORM = {-transform, transform};
}


void TileMap::injectMouse(int x, int y)
{
	mMousePosition = {x, y};
}


void TileMap::mapViewLocation(NAS2D::Point<int> point)
{
	mMapViewLocation = {
		std::clamp(point.x(), 0, mWidth - mEdgeLength),
		std::clamp(point.y(), 0, mHeight - mEdgeLength)
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

	mapViewLocation({
		std::clamp(_t->x() - edgeLength() / 2, 0, mWidth - 1),
		std::clamp(_t->y() - edgeLength() / 2, 0, mHeight - 1)
	});
	currentDepth(_t->depth());
}


/**
 * Returns true if the current tile highlight is actually within the visible diamond map.
 */
bool TileMap::tileHighlightVisible() const
{
	return NAS2D::Rectangle<int>::Create(mMapViewLocation, NAS2D::Vector{mEdgeLength - 1, mEdgeLength - 1}).contains(mMapHighlight);
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
			Tile& tile = mTileMap[mCurrentDepth][row + mMapViewLocation.y()][col + mMapViewLocation.x()];

			if (tile.excavated())
			{
				const auto position = mMapPosition + NAS2D::Vector{(col - row) * TILE_HALF_WIDTH, (col + row) * TILE_HEIGHT_HALF_ABSOLUTE};
				const auto subImageRect = NAS2D::Rectangle{tile.index() * TILE_WIDTH, tsetOffset, TILE_WIDTH, TILE_HEIGHT};
				const bool isTileHighlighted = NAS2D::Vector{col, row} == highlightOffset;
				const bool isConnectionHighlighted = mShowConnections && tile.connected();
				const NAS2D::Color highlightColor =
					isTileHighlighted ?
						isConnectionHighlighted ? NAS2D::Color{71, 224, 146} : NAS2D::Color{125, 200, 255} :
						isConnectionHighlighted ? NAS2D::Color::Green : NAS2D::Color::Normal;
				renderer.drawSubImage(mTileset, position, subImageRect, highlightColor);

				// Draw a beacon on an unoccupied tile with a mine
				if (tile.mine() != nullptr && !tile.thing())
				{
					uint8_t glow = 120 + sin(mTimer.tick() / THROB_SPEED) * 57;
					const auto mineBeaconPosition = position + NAS2D::Vector{TILE_HALF_WIDTH - 6, 15};

					renderer.drawImage(mMineBeacon, mineBeaconPosition);
					renderer.drawSubImage(mMineBeacon, mineBeaconPosition, NAS2D::Rectangle{0, 0, 10, 5}, NAS2D::Color{glow, glow, glow});
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
	int even_edge_length_adjust = 0;
	if (edgeLength() % 2 == 0) { even_edge_length_adjust = TILE_HALF_WIDTH; }

	int offsetX = ((mMousePosition.x() - mMapBoundingBox.x() - even_edge_length_adjust) / TILE_WIDTH);
	int offsetY = ((mMousePosition.y() - mMapBoundingBox.y()) / TILE_HEIGHT_ABSOLUTE);
	NAS2D::Vector<int> highlightOffset = {TRANSFORM.x() + offsetY + offsetX, TRANSFORM.y() + offsetY - offsetX};

	int mmOffsetX = std::clamp((mMousePosition.x() - mMapBoundingBox.x() - even_edge_length_adjust) % TILE_WIDTH, 0, TILE_WIDTH);
	int mmOffsetY = (mMousePosition.y() - mMapBoundingBox.y()) % TILE_HEIGHT_ABSOLUTE;

	MouseMapRegion mmr = getMouseMapRegion(mmOffsetX, mmOffsetY);

	switch (mmr)
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
	return mMouseMap[y][x];
}


static void serializeTile(XmlElement* _ti, int x, int y, int depth, int index)
{
	XmlElement* t = new XmlElement("tile");
	t->attribute("x", x);
	t->attribute("y", y);
	t->attribute("depth", depth);
	t->attribute("index", index);

	_ti->linkEndChild(t);
}


void TileMap::serialize(NAS2D::Xml::XmlElement* element)
{
	// ==========================================
	// MAP PROPERTIES
	// ==========================================
	XmlElement *properties = new XmlElement("properties");
	element->linkEndChild(properties);

	properties->attribute("sitemap", mMapPath);
	properties->attribute("tset", mTsetPath);
	properties->attribute("diggingdepth", mMaxDepth);

	// ==========================================
	// VIEW PARAMETERS
	// ==========================================
	XmlElement *viewparams = new XmlElement("view_parameters");
	element->linkEndChild(viewparams);

	viewparams->attribute("currentdepth", mCurrentDepth);
	viewparams->attribute("viewlocation_x", mMapViewLocation.x());
	viewparams->attribute("viewlocation_y", mMapViewLocation.y());

	// ==========================================
	// MINES
	// ==========================================
	XmlElement *mines = new XmlElement("mines");
	element->linkEndChild(mines);

	for (std::size_t i = 0; i < mMineLocations.size(); ++i)
	{
		XmlElement *mine = new XmlElement("mine");
		mine->attribute("x", mMineLocations[i].x());
		mine->attribute("y", mMineLocations[i].y());
		getTile(mMineLocations[i], TileMapLevel::LEVEL_SURFACE)->mine()->serialize(mine);
		mines->linkEndChild(mine);
	}


	// ==========================================
	// TILES
	// ==========================================
	XmlElement *tiles = new XmlElement("tiles");
	element->linkEndChild(tiles);

	// We're only writing out tiles that don't have structures or robots in them that are
	// underground and excavated or surface and bulldozed.
	Tile* tile = nullptr;
	for (int depth = 0; depth <= maxDepth(); ++depth)
	{
		for (int x = 0; x < width(); ++x)
		{
			for (int y = 0; y < height(); ++y)
			{
				tile = getTile({x, y}, depth);
				if (depth > 0 && tile->excavated() && tile->empty() && tile->mine() == nullptr)
				{
					serializeTile(tiles, x, y, depth, tile->index());
				}
				else if (tile->index() == 0 && tile->empty() && tile->mine() == nullptr)
				{
					serializeTile(tiles, x, y, depth, tile->index());
				}
			}
		}
	}
}


void TileMap::deserialize(NAS2D::Xml::XmlElement* element)
{
	// VIEW PARAMETERS
	int view_x = 0, view_y = 0, view_depth = 0;
	XmlElement* view_parameters = element->firstChildElement("view_parameters");
	XmlAttribute* attribute = view_parameters->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "viewlocation_x") { attribute->queryIntValue(view_x); }
		else if (attribute->name() == "viewlocation_y") { attribute->queryIntValue(view_y); }
		else if (attribute->name() == "currentdepth") { attribute->queryIntValue(view_depth); }
		attribute = attribute->next();
	}

	mapViewLocation({view_x, view_y});
	currentDepth(view_depth);
	for (XmlNode* mine = element->firstChildElement("mines")->firstChildElement("mine"); mine; mine = mine->nextSibling())
	{
		int x = 0, y = 0;
	
		attribute = mine->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x") { attribute->queryIntValue(x); }
			else if (attribute->name() == "y") { attribute->queryIntValue(y); }
			attribute = attribute->next();
		}

		Mine* m = new Mine();
		m->deserialize(mine->toElement());

		mTileMap[0][y][x].pushMine(m);
		mTileMap[0][y][x].index(TerrainType::TERRAIN_DOZED);

		mMineLocations.push_back(Point{x, y});

		/// \fixme	Legacy code to assist in updating older versions of save games between 0.7.5 and 0.7.6. Remove in 0.8.0
		if (m->depth() == 0 && m->active()) { m->increaseDepth(); }
	}

	// TILES AT INDEX 0 WITH NO THING'S
	for (XmlNode* tile = element->firstChildElement("tiles")->firstChildElement("tile"); tile; tile = tile->nextSibling())
	{
		int x = 0, y = 0, depth = 0, index = 0;

		attribute = tile->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x") { attribute->queryIntValue(x); }
			else if (attribute->name() == "y") { attribute->queryIntValue(y); }
			else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
			else if (attribute->name() == "index") { attribute->queryIntValue(index); }

			attribute = attribute->next();
		}

		mTileMap[depth][y][x].index(static_cast<TerrainType>(index));

		if (depth > 0) { mTileMap[depth][y][x].excavated(true); }
	}
}


Tile* TileMap::getVisibleTile(NAS2D::Point<int> position, int level)
{
	if (!isVisibleTile(position, level))
	{
		return nullptr;
	}

	return getTile(position, level);
}


bool TileMap::isVisibleTile(NAS2D::Point<int> position, int z) const
{
	if (!NAS2D::Rectangle{mMapViewLocation.x(), mMapViewLocation.y(), mEdgeLength - 1, mEdgeLength - 1}.contains(position))
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


using namespace micropather;
void TileMap::AdjacentCost(void* state, std::vector<StateCost>* adjacent)
{
	Tile* tile = static_cast<Tile*>(state);

	const auto tilePosition = tile->position();

	for (const auto& offset : DirectionClockwise4)
	{
		Tile* adjacent_tile = getTile(tilePosition + offset, 0);
		float cost = 0.5f;

		if (!adjacent_tile || !adjacent_tile->empty() || adjacent_tile->index() == TerrainType::TERRAIN_IMPASSABLE) { cost = FLT_MAX; }
		else { cost *= static_cast<float>(adjacent_tile->index()) + 1.0f; }

		StateCost nodeCost = { adjacent_tile, cost };
		adjacent->push_back(nodeCost);
	}
}
