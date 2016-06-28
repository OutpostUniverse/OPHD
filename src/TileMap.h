#pragma once

#include "Tile.h"

#include "Things/Structures/Structure.h"

typedef vector<Point_2d> Point2dList;

class TileMap
{
public:

	enum TileMapLevel
	{
		LEVEL_SURFACE = 0,
		LEVEL_UG_1,
		LEVEL_UG_2,
		LEVEL_UG_3,
		LEVEL_UG_4
	};

	
	TileMap(const std::string& map_path, const std::string& tset_path, int maxDepth);

	~TileMap();

	Tile* getTile(int x, int y, int level);
	Tile* getTile(int x, int y) { return getTile(x, y, mCurrentDepth); }

	const Rectangle_2d& boundingBox() const { return mMapBoundingBox; }

	const Point_2d& mapViewLocation() const { return mMapViewLocation; }
	void mapViewLocation(int x, int y) { mMapViewLocation(x, y); }

	const Point_2d& tileHighlight() const { return mMapHighlight; }
	bool tileHighlightVisible() const;

	int tileMouseHoverX() const { return mMapHighlight.x() + mMapViewLocation.x(); }
	int tileMouseHoverY() const { return mMapHighlight.y() + mMapViewLocation.y(); }

	const Point2dList& mineLocations() const { return mMineLocations; }

	void toggleShowConnections() { mShowConnections = !mShowConnections; }

	int edgeLength() const { return mEdgeLength; }
	int width() const { return mWidth; }
	int height() const { return mHeight; }

	int currentDepth() const { return mCurrentDepth; }
	void currentDepth(int _i) { mCurrentDepth = clamp(_i, 0, mMaxDepth); }

	int maxDepth() const { return mMaxDepth; }

	void injectMouse(int x, int y);

	void draw();

	void serialize(TiXmlElement* _ti);

protected:
	enum MouseMapRegion
	{
		MMR_MIDDLE,
		MMR_TOP_RIGHT,
		MMR_TOP_LEFT,
		MMR_BOTTOM_RIGHT,
		MMR_BOTTOM_LEFT
	};
	
	vector<vector<MouseMapRegion> > mMouseMap;

private:

	typedef vector<vector<Tile> >	TileGrid;
	typedef vector<TileGrid>		TileArray;
	
	void buildMouseMap();
	void buildTerrainMap(const std::string& path);
	void setupMines();

	void initMapDrawParams();

	void updateTileHighlight();

	MouseMapRegion getMouseMapRegion(int x, int y);

	int					mEdgeLength;
	int					mWidth, mHeight;

	int					mMaxDepth;			/**< Maximum digging depth. */
	int					mCurrentDepth;		/**< Current depth level to view. */

	string				mMapPath;
	string				mTsetPath;

	TileArray			mTileMap;

	Image				mTileSelector;
	Image				mTileset;

	Sprite				mMineBeacon;

	Point_2d			mMousePosition;		/**< Current mouse position. */
	Point_2d			mMapHighlight;		/**< Tile the mouse is pointing to. */
	Point_2d			mMapViewLocation;	/**<  */

	Point_2df			mMapPosition;		/** Where to start drawing the TileMap on the screen. */

	Point2dList			mMineLocations;		/**< Location of all mines on the map. */

	Rectangle_2d		mMapBoundingBox;	/** Area that the TileMap fills when drawn. */

	bool				mDebug;
	bool				mShowConnections;	/**< Flag indicating whether or not to highlight connectedness. */

};
