#include "TileMap.h"

#include "Constants.h"


#pragma warning(disable: 4244)

const std::string	MAP_TERRAIN_EXTENSION		= "_a.png";

Point_2d			TRANSFORM;							// FIXME: Document what I do.

const int			MAP_WIDTH					= 300;
const int			MAP_HEIGHT					= 150;

const int			TILE_WIDTH					= 107;
const int			TILE_HEIGHT					= 46;

const double		THROB_SPEED					= 250.0f; // Throb speed of mine beacon

TileMap::TileMap(const string& map_path, const string& tset_path, int _md, int _mc, bool _s):	mEdgeLength(0),
																								mWidth(MAP_WIDTH), mHeight(MAP_HEIGHT),
																								mMaxDepth(_md),
																								mCurrentDepth(0),
																								mTileSelector("ui/selector.png"),
																								mTileset(tset_path),
																								mMineBeacon("structures/mine_beacon.png"),
																								mDebug(false),
																								mShowConnections(false)
{
	buildTerrainMap(map_path);
	buildMouseMap();
	initMapDrawParams();

	if(_s)
		setupMines(_mc);

	mMapPath = map_path;
	mTsetPath = tset_path;
}


TileMap::~TileMap()
{}


Tile* TileMap::getTile(int x, int y, int level)
{
	if(x >= 0 && x < width() && y >= 0 && y < height() && level >= 0 && level <= mMaxDepth)
		return &mTileMap[level][y][x];

	return nullptr;
}


/**
 * Builds the terrain map.
 */
void TileMap::buildTerrainMap(const std::string& path)
{
	if(!Utility<Filesystem>::get().exists(path + MAP_TERRAIN_EXTENSION))
		throw Exception(0, "Map Not Found", "Given map file does not exist.");

	Image heightmap(path + MAP_TERRAIN_EXTENSION);

	mTileMap.resize(mMaxDepth + 1);

	for(int level = 0; level <= mMaxDepth; level++)
	{
		mTileMap[level].resize(height());
		for(size_t i = 0; i < mTileMap[level].size(); i++)
			mTileMap[level][i].resize(width());
	}

	// Build map based on terrain map
	for(int depth = 0; depth <= mMaxDepth; depth++)
	{
		for(int row = 0; row < height(); row++)
		{
			for(int col = 0; col < width(); col++)
			{
				Color_4ub c = heightmap.pixelColor(col, row);
				Tile& t = mTileMap[depth][row][col];
				t.init(col, row, depth, c.red() / 50);
				if (depth > 0)
					t.excavated(false);
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
		Point_2d pt(Random::ranged_integer(5, MAP_WIDTH - 5), Random::ranged_integer(5, MAP_HEIGHT - 5));

		// Ugly
		if(mTileMap[0][pt.y()][pt.x()].mine())
			continue;

		float probability = 0.05f * mTileMap[0][pt.y()][pt.x()].index();

		if(Random::ranged_integer(0, 100) <= (int)(probability * 100))
		{
			Mine* m = nullptr;
			
			// Choose a production rate
			// FIXME: Kind of a naive approach to this... would be nice to weight things better.
			if(Random::ranged_integer(0, 100) < 60)
				m = new Mine(Mine::PRODUCTION_RATE_MEDIUM);
			else if(Random::ranged_integer(0, 100) < 30)
				m = new Mine(Mine::PRODUCTION_RATE_HIGH);
			else
				m = new Mine(Mine::PRODUCTION_RATE_LOW);
			
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
	if(!Utility<Filesystem>::get().exists("ui/mouse_map.png"))
		throw Exception(0, "System File Error", "Unable to find the mouse map file.");

	Image mousemap("ui/mouse_map.png");

	// More sanity checks (mousemap should match dimensions of tile)
	if(mousemap.width() != TILE_WIDTH || mousemap.height() != TILE_HEIGHT)
		throw Exception(0, "System File Error", "Mouse map is the wrong dimensions.");
	
	mMouseMap.resize(TILE_HEIGHT);
	for(size_t i = 0; i < mMouseMap.size(); i++)
		mMouseMap[i].resize(TILE_WIDTH);

	for(size_t row = 0; row < TILE_HEIGHT; row++)
	{
		for(size_t col = 0; col < TILE_WIDTH; col++)
		{
			Color_4ub c = mousemap.pixelColor(col, row);
			if(c.red() == 255 && c.green() == 255)
				mMouseMap[row][col] = MMR_BOTTOM_RIGHT;
			else if(c.red() == 255)
				mMouseMap[row][col] = MMR_TOP_LEFT;
			else if(c.blue() == 255)
				mMouseMap[row][col] = MMR_TOP_RIGHT;
			else if(c.green() == 255)
				mMouseMap[row][col] = MMR_BOTTOM_LEFT;
			else
				mMouseMap[row][col] = MMR_MIDDLE;
		}
	}
}


/**
 * Sets up position and drawing parememters for the tile map.
 */
void TileMap::initMapDrawParams()
{
	// Set up map draw position
	float screenW = Utility<Renderer>::get().width(), screenH = Utility<Renderer>::get().height();

	mEdgeLength = static_cast<int>(screenW) / TILE_WIDTH;

	mMapPosition((screenW / 2 - (TILE_WIDTH / 2)), ((screenH - constants::BOTTOM_UI_HEIGHT) / 2) - ((static_cast<float>(mEdgeLength) / 2) * TILE_HEIGHT));
	mMapBoundingBox((screenW / 2) - ((TILE_WIDTH * mEdgeLength) / 2), mMapPosition.y(), TILE_WIDTH * mEdgeLength, TILE_HEIGHT * mEdgeLength);

	int transform = (mMapPosition.x() - mMapBoundingBox.x()) / TILE_WIDTH;
	TRANSFORM(-transform, transform);
}


void TileMap::injectMouse(int x, int y)
{
	mMousePosition(x, y);
}


/**
 * Returns true if the current tile highlight is actually within the visible diamond map.
 */
bool TileMap::tileHighlightVisible() const
{
	return isPointInRect(mMapHighlight.x(), mMapHighlight.y(), 0, 0, mEdgeLength, mEdgeLength);
}


void TileMap::draw()
{
	Renderer& r = Utility<Renderer>::get();

	int x = 0, y = 0;
	Tile* tile = NULL;

	int tsetOffset = 0;

	mCurrentDepth > 0 ? tsetOffset = 1 : tsetOffset = 0;

	for(int row = 0; row < mEdgeLength; row ++)
	{
		for(int col = 0; col < mEdgeLength; col++)
		{
			x = mMapPosition.x() + ((col - row) * (TILE_WIDTH / 2));
			y = mMapPosition.y() + (col + row) * (TILE_HEIGHT / 2);

			tile = &mTileMap[mCurrentDepth][row + mMapViewLocation.y()][col + mMapViewLocation.x()];

			if(tile->excavated())
			{
				if (mShowConnections)
				{
					if (tile->connected())
						r.drawSubImage(mTileset, x, y, tile->index() * TILE_WIDTH, tsetOffset * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, 0, 255, 0, 255);
					else
						r.drawSubImage(mTileset, x, y, tile->index() * TILE_WIDTH, tsetOffset * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
				}
				else
					r.drawSubImage(mTileset, x, y, tile->index() * TILE_WIDTH, tsetOffset * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);

				if(row == mMapHighlight.y() && col == mMapHighlight.x())
					r.drawImage(mTileSelector,	mMapPosition.x() + ((mMapHighlight.x() - mMapHighlight.y()) * (TILE_WIDTH / 2)),
												mMapPosition.y() + (mMapHighlight.x() + mMapHighlight.y()) * (TILE_HEIGHT / 2));

				// Draw a beacon on an unoccupied tile with a mine
				if (tile->mine() != NULL && !tile->thing())
				{
					int glow = 120 + sin(mTimer.tick() / THROB_SPEED) * 57, loc_x = x + (TILE_WIDTH >> 1) - 6, loc_y = y + (TILE_HEIGHT >> 1) - 22;

					r.drawImage(mMineBeacon, loc_x, loc_y);
					r.drawSubImage(mMineBeacon, loc_x, loc_y, 0, 0, 10, 5, glow, glow, glow, 255);
				}

				// Tell an occupying thing to update itself.
				if(tile->thing())
					tile->thing()->sprite().update(x, y);
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
	Renderer& r = Utility<Renderer>::get();

	if(isPointInRect(mMousePosition, mMapBoundingBox))
	{
		int offsetX = (mMousePosition.x() - mMapBoundingBox.x()) / TILE_WIDTH;
		int offsetY = (mMousePosition.y() - mMapBoundingBox.y()) / TILE_HEIGHT;
		mMapHighlight(TRANSFORM.x() + offsetY + offsetX, TRANSFORM.y() + offsetY - offsetX);

		//Point_2d pt((mMousePosition.x() - mMapBoundingBox.x()) % TILE_WIDTH, (mMousePosition.y() - mMapBoundingBox.y()) % TILE_HEIGHT);
		MouseMapRegion mmr = getMouseMapRegion((mMousePosition.x() - mMapBoundingBox.x()) % TILE_WIDTH, (mMousePosition.y() - mMapBoundingBox.y()) % TILE_HEIGHT);

		switch(mmr)
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

		//if(mMapHighlight.x() >= 0 && mMapHighlight.x() < mEdgeLength && mMapHighlight.y() >= 0 && mMapHighlight.y() < mEdgeLength)
			//r.drawImage(mTileSelector,	mMapPosition.x() + ((mMapHighlight.x() - mMapHighlight.y()) * (TILE_WIDTH / 2)),
										//mMapPosition.y() + (mMapHighlight.x() + mMapHighlight.y()) * (TILE_HEIGHT / 2));
	}
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


void serializeTile(TiXmlElement* _ti, int x, int y, int depth, int index)
{
	TiXmlElement* t = new TiXmlElement("tile");
	t->SetAttribute("x", x);
	t->SetAttribute("y", y);
	t->SetAttribute("depth", depth);
	t->SetAttribute("index", index);

	_ti->LinkEndChild(t);
}


void TileMap::serialize(TiXmlElement* _ti)
{
	// ==========================================
	// MAP PROPERTIES
	// ==========================================
	TiXmlElement *properties = new TiXmlElement("properties");
	_ti->LinkEndChild(properties);

	properties->SetAttribute("sitemap", mMapPath);
	properties->SetAttribute("tset", mTsetPath);
	properties->SetAttribute("diggingdepth", mMaxDepth);

	// ==========================================
	// VIEW PARAMETERS
	// ==========================================
	TiXmlElement *viewparams = new TiXmlElement("view_parameters");
	_ti->LinkEndChild(viewparams);

	viewparams->SetAttribute("currentdepth", mCurrentDepth);
	viewparams->SetAttribute("viewlocation_x", mMapViewLocation.x());
	viewparams->SetAttribute("viewlocation_y", mMapViewLocation.y());


	// ==========================================
	// MINES
	// ==========================================
	TiXmlElement *mines = new TiXmlElement("mines");
	_ti->LinkEndChild(mines);

	for (size_t i = 0; i < mMineLocations.size(); ++i)
	{
		TiXmlElement *mine = new TiXmlElement("mine");
		mine->SetAttribute("x", mMineLocations[i].x());
		mine->SetAttribute("y", mMineLocations[i].y());
		getTile(mMineLocations[i].x(), mMineLocations[i].y(), LEVEL_SURFACE)->mine()->serialize(mine);
		mines->LinkEndChild(mine);
	}


	// ==========================================
	// TILES
	// ==========================================
	TiXmlElement *tiles = new TiXmlElement("tiles");
	_ti->LinkEndChild(tiles);

	// We're only writing out tiles that don't have structures or robots in them that are
	// underground and excavated or surface and bulldozed.
	Tile* tile = nullptr;
	for (size_t depth = 0; depth < maxDepth(); ++depth)
	{
		for (size_t x = 0; x < width(); ++x)
		{
			for (size_t y = 0; y < height(); ++y)
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


void TileMap::deserialize(TiXmlElement* _ti)
{
	// VIEW PARAMETERS
	TiXmlElement* view_parameters = _ti->FirstChildElement("view_parameters");
	int view_x = 0, view_y = 0, view_depth = 0;
	view_parameters->Attribute("viewlocation_x", &view_x);
	view_parameters->Attribute("viewlocation_y", &view_y);
	view_parameters->Attribute("currentdepth", &view_depth);

	mapViewLocation(view_x, view_y);
	currentDepth(view_depth);

	for (TiXmlNode* mine = _ti->FirstChildElement("mines")->FirstChildElement("mine"); mine != nullptr; mine = mine->NextSibling())
	{
		int x = 0, y = 0, age = 0, depth = 0, active = 0, exhausted = 0, yield = 0;

		mine->ToElement()->Attribute("x", &x);
		mine->ToElement()->Attribute("y", &y);
		mine->ToElement()->Attribute("age", &age);
		mine->ToElement()->Attribute("depth", &depth);
		mine->ToElement()->Attribute("active", &active);
		mine->ToElement()->Attribute("exhausted", &exhausted);
		mine->ToElement()->Attribute("yield", &yield);

		Mine* m = new Mine(static_cast<Mine::ProductionRate>(yield));
		m->age(age);
		m->depth(depth);
		m->active(active > 0);
		m->exhausted(exhausted > 0);

		mTileMap[0][y][x].pushMine(m);
		mTileMap[0][y][x].index(TERRAIN_DOZED);

		mMineLocations.push_back(Point_2d(x, y));
	}

	// TILES AT INDEX 0 WITH NO THING'S
	for (TiXmlNode* tile = _ti->FirstChildElement("tiles")->FirstChildElement("tile"); tile != nullptr; tile = tile->NextSibling())
	{
		int x = 0, y = 0, depth = 0, index = 0;
		tile->ToElement()->Attribute("x", &x);
		tile->ToElement()->Attribute("y", &y);
		tile->ToElement()->Attribute("depth", &depth);
		tile->ToElement()->Attribute("index", &index);

		mTileMap[depth][y][x].index(static_cast<TerrainType>(index));

		if (depth > 0)
			mTileMap[depth][y][x].excavated(true);
	}
}