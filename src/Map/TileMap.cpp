// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TileMap.h"

#include "../Constants.h"

#include <algorithm>
#include <functional>
#include <random>

using namespace NAS2D;
using namespace NAS2D::Xml;

#pragma warning(disable: 4244)

// ===============================================================================
// = CONSTANTS
// ===============================================================================
const std::string	MAP_TERRAIN_EXTENSION		= "_a.png";

const int			MAP_WIDTH					= 300;
const int			MAP_HEIGHT					= 150;

const int			TILE_WIDTH					= 128;
const int			TILE_HEIGHT					= 64;

const int			TILE_HALF_WIDTH				= TILE_WIDTH / 2;

const int			TILE_HEIGHT_OFFSET			= 9;
const int			TILE_HEIGHT_ABSOLUTE		= TILE_HEIGHT - TILE_HEIGHT_OFFSET;
const int			TILE_HEIGHT_HALF_ABSOLUTE	= TILE_HEIGHT_ABSOLUTE / 2;

const double		THROB_SPEED					= 250.0f; // Throb speed of mine beacon


// ===============================================================================
// = LOCAL VARIABLES
// ===============================================================================
Point_2d			TRANSFORM; /**< Used to adjust mouse and screen spaces based on position of the map field. */


// ===============================================================================
// = RANDOM NUMBER GENERATION
// ===============================================================================
std::random_device rd;
std::mt19937 generator(rd());
std::uniform_int_distribution<int> map_width(5, MAP_WIDTH - 5);
std::uniform_int_distribution<int> map_height(5, MAP_HEIGHT - 5);
std::uniform_int_distribution<int> mine_yield(0, 100);

auto mwidth = std::bind(map_width, std::ref(generator));
auto mheight = std::bind(map_height, std::ref(generator));
auto myield = std::bind(mine_yield, std::ref(generator));


/**
 * C'tor
 */
TileMap::TileMap(const std::string& map_path, const std::string& tset_path, int _md, int _mc, bool _s) :
	mWidth(MAP_WIDTH), mHeight(MAP_HEIGHT),	mMaxDepth(_md),
	mMapPath(map_path), mTsetPath(tset_path),
	mTileSelector("ui/selector.png"),
	mTileset(tset_path),
	mMineBeacon("structures/mine_beacon.png")
{
	std::cout << "Loading '" << map_path << "'... ";
	buildTerrainMap(map_path);
	buildMouseMap();
	initMapDrawParams(Utility<Renderer>::get().width(), Utility<Renderer>::get().height());

	if (_s) { setupMines(_mc); }
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
void TileMap::removeMineLocation(const NAS2D::Point_2d& pt)
{
	mMineLocations.erase(find(mMineLocations.begin(), mMineLocations.end(), pt));
	getTile(pt.x(), pt.y(), 0)->pushMine(nullptr);
}


Tile* TileMap::getTile(int x, int y, int level)
{
	if (x >= 0 && x < width() && y >= 0 && y < height() && level >= 0 && level <= mMaxDepth)
	{
		return &mTileMap[level][y][x];
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

	mTileMap.resize(mMaxDepth + 1);
	for(int level = 0; level <= mMaxDepth; level++)
	{
		mTileMap[level].resize(height());
		for (size_t i = 0; i < mTileMap[level].size(); i++)
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
				Color_4ub c = heightmap.pixelColor(col, row);
				Tile& t = mTileMap[depth][row][col];
				t.init(col, row, depth, c.red() / 50);
				if (depth > 0) { t.excavated(false); }
			}
		}
	}
}


/**
 * Creates mining locations around the map area.
 */
void TileMap::setupMines(int mineCount)
{
	int i = 0;
	while(i < mineCount)
	{
		Point_2d pt(mwidth(), mheight());

		
		if (mTileMap[0][pt.y()][pt.x()].mine()) { continue; } // Ugly

		float probability = 0.05f * mTileMap[0][pt.y()][pt.x()].index();

		if(myield() <= (int)(probability * 100))
		{
			Mine* m = nullptr;
			
			// Choose a production rate
			// FIXME: Kind of a naive approach to this... would be nice to weight things better.
			if (myield() < 60) { m = new Mine(PRODUCTION_RATE_MEDIUM); }
			else if (myield() < 30) { m = new Mine(PRODUCTION_RATE_HIGH); }
			else { m = new Mine(PRODUCTION_RATE_LOW); }
			
			mTileMap[0][pt.y()][pt.x()].pushMine(m);
			mTileMap[0][pt.y()][pt.x()].index(TERRAIN_DOZED);

			mMineLocations.push_back(pt);
			++i;
		}
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
	if (mousemap.width() != TILE_WIDTH || mousemap.height() != TILE_HEIGHT_ABSOLUTE)
	{
		throw std::runtime_error("Mouse map is the wrong dimensions.");
	}
	
	mMouseMap.resize(TILE_HEIGHT_ABSOLUTE);
	for (size_t i = 0; i < mMouseMap.size(); i++)
	{
		mMouseMap[i].resize(TILE_WIDTH);
	}

	for(size_t row = 0; row < TILE_HEIGHT_ABSOLUTE; row++)
	{
		for(size_t col = 0; col < TILE_WIDTH; col++)
		{
			Color_4ub c = mousemap.pixelColor(col, row);
			if (c.red() == 255 && c.green() == 255)	{ mMouseMap[row][col] = MMR_BOTTOM_RIGHT; }
			else if (c.red() == 255)				{ mMouseMap[row][col] = MMR_TOP_LEFT; }
			else if (c.blue() == 255)				{ mMouseMap[row][col] = MMR_TOP_RIGHT; }
			else if (c.green() == 255)				{ mMouseMap[row][col] = MMR_BOTTOM_LEFT; }
			else									{ mMouseMap[row][col] = MMR_MIDDLE; }
		}
	}
}


/**
 * Sets up position and drawing parememters for the tile map.
 */
void TileMap::initMapDrawParams(int w, int h)
{
	// Set up map draw position
	mEdgeLength = w / TILE_WIDTH;

	mMapPosition((w / 2 - (TILE_WIDTH / 2)), ((h - constants::BOTTOM_UI_HEIGHT) / 2) - ((static_cast<float>(mEdgeLength) / 2) * TILE_HEIGHT_ABSOLUTE));
	mMapBoundingBox((w / 2) - ((TILE_WIDTH * mEdgeLength) / 2), mMapPosition.y(), TILE_WIDTH * mEdgeLength, TILE_HEIGHT_ABSOLUTE * mEdgeLength);

	int transform = (mMapPosition.x() - mMapBoundingBox.x()) / TILE_WIDTH;
	TRANSFORM(-transform, transform);
}


void TileMap::injectMouse(int x, int y)
{
	mMousePosition(x, y);
}


/**
 * Convenience function to focus the TileMap's view on a specified tile.
 * 
 * \param	_t	Pointer to a Tile. Safe to pass nullptr.
 */
void TileMap::centerMapOnTile(Tile* _t)
{
	if (!_t) { return; }

	mapViewLocation(clamp(_t->x() - edgeLength() / 2, 0, mWidth - 1),
					clamp(_t->y() - edgeLength() / 2, 0, mHeight - 1));
	currentDepth(_t->depth());
}


/**
 * Returns true if the current tile highlight is actually within the visible diamond map.
 */
bool TileMap::tileHighlightVisible() const
{
	return isPointInRect(mMapHighlight.x(), mMapHighlight.y(), 0, 0, mEdgeLength - 1, mEdgeLength - 1);
}


void TileMap::draw()
{
	Renderer& r = Utility<Renderer>::get();

	int x = 0, y = 0;
	Tile* tile = nullptr;

	int tsetOffset = 0;

	tsetOffset = mCurrentDepth > 0 ? TILE_HEIGHT : 0;

	for(int row = 0; row < mEdgeLength; row++)
	{
		for(int col = 0; col < mEdgeLength; col++)
		{
			x = mMapPosition.x() + ((col - row) * TILE_HALF_WIDTH);
			y = mMapPosition.y() + ((col + row) * TILE_HEIGHT_HALF_ABSOLUTE);

			tile = &mTileMap[mCurrentDepth][row + mMapViewLocation.y()][col + mMapViewLocation.x()];

			if(tile->excavated())
			{
				if (mShowConnections)
				{
					if (tile->connected())
					{
						r.drawSubImage(mTileset, x, y, tile->index() * TILE_WIDTH, tsetOffset, TILE_WIDTH, TILE_HEIGHT, 0, 255, 0, 255);
					}
					else
					{
						r.drawSubImage(mTileset, x, y, tile->index() * TILE_WIDTH, tsetOffset, TILE_WIDTH, TILE_HEIGHT);
					}
				}
				else
				{
					if (row == mMapHighlight.y() && col == mMapHighlight.x())
					{
						r.drawSubImage(mTileset, x, y, tile->index() * TILE_WIDTH, tsetOffset, TILE_WIDTH, TILE_HEIGHT, 125, 200, 255, 255);
					}
					else
					{
						r.drawSubImage(mTileset, x, y, tile->index() * TILE_WIDTH, tsetOffset, TILE_WIDTH, TILE_HEIGHT);
					}
				}

				/*
				if (row == mMapHighlight.y() && col == mMapHighlight.x())
				{
					r.drawImage(mTileSelector,
								mMapPosition.x() + ((mMapHighlight.x() - mMapHighlight.y()) * TILE_HALF_WIDTH),
								mMapPosition.y() + (mMapHighlight.x() + mMapHighlight.y()) * TILE_HEIGHT_HALF_ABSOLUTE);
				}
				*/

				// Draw a beacon on an unoccupied tile with a mine
				if (tile->mine() != nullptr && !tile->thing())
				{
					int glow = 120 + sin(mTimer.tick() / THROB_SPEED) * 57;
					int loc_x = x + TILE_HALF_WIDTH - 6;
					int loc_y = y + 15;

					r.drawImage(mMineBeacon, loc_x, loc_y);
					r.drawSubImage(mMineBeacon, loc_x, loc_y, 0, 0, 10, 5, glow, glow, glow, 255);
				}

				// Tell an occupying thing to update itself.
				if (tile->thing()) { tile->thing()->sprite().update(x, y); }
			}
		}
	}

	if(mDebug) r.drawBox(mMapBoundingBox, 0, 200, 0);

	updateTileHighlight();
}


/**
 * Brute Force but works.
 */
void TileMap::updateTileHighlight()
{
	if (!isPointInRect(mMousePosition, mMapBoundingBox))
	{
		return;
	}

	/// In the case of even edge lengths, we need to adjust the mouse picking code a bit.
	int even_edge_length_adjust = 0;
	if (edgeLength() % 2 == 0) { even_edge_length_adjust = TILE_HALF_WIDTH; }

	int offsetX = ((mMousePosition.x() - mMapBoundingBox.x() - even_edge_length_adjust) / TILE_WIDTH);
	int offsetY = ((mMousePosition.y() - mMapBoundingBox.y()) / TILE_HEIGHT_ABSOLUTE);
	mMapHighlight(TRANSFORM.x() + offsetY + offsetX, TRANSFORM.y() + offsetY - offsetX);

	int mmOffsetX = clamp((mMousePosition.x() - mMapBoundingBox.x() - even_edge_length_adjust) % TILE_WIDTH, 0, TILE_WIDTH);
	int mmOffsetY = (mMousePosition.y() - mMapBoundingBox.y()) % TILE_HEIGHT_ABSOLUTE;

	MouseMapRegion mmr = getMouseMapRegion(mmOffsetX, mmOffsetY);

	switch (mmr)
	{
	case MMR_TOP_RIGHT:
		mMapHighlight.y(--mMapHighlight.y());
		break;

	case MMR_TOP_LEFT:
		mMapHighlight.x(--mMapHighlight.x());
		break;

	case MMR_BOTTOM_RIGHT:
		mMapHighlight.x(++mMapHighlight.x());
		break;

	case MMR_BOTTOM_LEFT:
		mMapHighlight.y(++mMapHighlight.y());
		break;

	default:
		break;
	};
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


void TileMap::serialize(XmlElement* _ti)
{
	// ==========================================
	// MAP PROPERTIES
	// ==========================================
	XmlElement *properties = new XmlElement("properties");
	_ti->linkEndChild(properties);

	properties->attribute("sitemap", mMapPath);
	properties->attribute("tset", mTsetPath);
	properties->attribute("diggingdepth", mMaxDepth);

	// ==========================================
	// VIEW PARAMETERS
	// ==========================================
	XmlElement *viewparams = new XmlElement("view_parameters");
	_ti->linkEndChild(viewparams);

	viewparams->attribute("currentdepth", mCurrentDepth);
	viewparams->attribute("viewlocation_x", mMapViewLocation.x());
	viewparams->attribute("viewlocation_y", mMapViewLocation.y());

	// ==========================================
	// MINES
	// ==========================================
	XmlElement *mines = new XmlElement("mines");
	_ti->linkEndChild(mines);

	for (size_t i = 0; i < mMineLocations.size(); ++i)
	{
		XmlElement *mine = new XmlElement("mine");
		mine->attribute("x", mMineLocations[i].x());
		mine->attribute("y", mMineLocations[i].y());
		getTile(mMineLocations[i].x(), mMineLocations[i].y(), LEVEL_SURFACE)->mine()->serialize(mine);
		mines->linkEndChild(mine);
	}


	// ==========================================
	// TILES
	// ==========================================
	XmlElement *tiles = new XmlElement("tiles");
	_ti->linkEndChild(tiles);

	// We're only writing out tiles that don't have structures or robots in them that are
	// underground and excavated or surface and bulldozed.
	Tile* tile = nullptr;
	for (int depth = 0; depth <= maxDepth(); ++depth)
	{
		for (int x = 0; x < width(); ++x)
		{
			for (int y = 0; y < height(); ++y)
			{
				tile = getTile(x, y, depth);
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


void TileMap::deserialize(XmlElement* _ti)
{
	// VIEW PARAMETERS
	int view_x = 0, view_y = 0, view_depth = 0;
	XmlElement* view_parameters = _ti->firstChildElement("view_parameters");
	XmlAttribute* attribute = view_parameters->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "viewlocation_x")		{ attribute->queryIntValue(view_x); }
		else if (attribute->name() == "viewlocation_y")	{ attribute->queryIntValue(view_y); }
		else if (attribute->name() == "currentdepth")	{ attribute->queryIntValue(view_depth); }
		attribute = attribute->next();
	}

	mapViewLocation(view_x, view_y);
	currentDepth(view_depth);
	for (XmlNode* mine = _ti->firstChildElement("mines")->firstChildElement("mine"); mine; mine = mine->nextSibling())
	{
		int x = 0, y = 0;
	
		attribute = mine->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x")		{ attribute->queryIntValue(x); }
			else if (attribute->name() == "y")	{ attribute->queryIntValue(y); }
			attribute = attribute->next();
		}

		Mine* m = new Mine();
		m->deserialize(mine->toElement());

		mTileMap[0][y][x].pushMine(m);
		mTileMap[0][y][x].index(TERRAIN_DOZED);

		mMineLocations.push_back(Point_2d(x, y));

		/// \fixme	Legacy code to assist in updating older versions of save games between 0.7.5 and 0.7.6. Remove in 0.8.0
		if (m->depth() == 0 && m->active()) { m->increaseDepth(); }
	}

	// TILES AT INDEX 0 WITH NO THING'S
	for (XmlNode* tile = _ti->firstChildElement("tiles")->firstChildElement("tile"); tile; tile = tile->nextSibling())
	{
		int x = 0, y = 0, depth = 0, index = 0;

		attribute = tile->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x")			{ attribute->queryIntValue(x); }
			else if (attribute->name() == "y")		{ attribute->queryIntValue(y); }
			else if (attribute->name() == "depth")	{ attribute->queryIntValue(depth); }
			else if (attribute->name() == "index")	{ attribute->queryIntValue(index); }

			attribute = attribute->next();
		}

		mTileMap[depth][y][x].index(static_cast<TerrainType>(index));

		if (depth > 0) { mTileMap[depth][y][x].excavated(true); }
	}
}


Tile* TileMap::getVisibleTile(int x, int y, int level)
{
	if (!isVisibleTile(x, y, level))
	{
		return nullptr;
	}

	return getTile(x, y, level);
}


bool TileMap::isVisibleTile(int _x, int _y, int _d) const
{
	if (!isPointInRect(_x, _y, mMapViewLocation.x(), mMapViewLocation.y(), mEdgeLength - 1, mEdgeLength - 1))
	{
		return false;
	}

	if (_d != mCurrentDepth)
	{
		return false;
	}
	
	return true;
}
