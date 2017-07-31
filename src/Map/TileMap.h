#pragma once

#include "Tile.h"

#include "../Things/Structures/Structure.h"

typedef std::vector<Point_2d> Point2dList;

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

	
	TileMap(const std::string& map_path, const std::string& tset_path, int maxDepth, int mineCount, bool setupMines = true);

	~TileMap();

	Tile* getTile(int x, int y, int level);
	Tile* getTile(int x, int y) { return getTile(x, y, mCurrentDepth); }
	
	Tile* getVisibleTile(int x, int y, int level);
	Tile* getVisibleTile(int x, int y) { return getVisibleTile(x, y, mCurrentDepth); }
	Tile* getVisibleTile() { return getVisibleTile(tileMouseHoverX(), tileMouseHoverY(), mCurrentDepth); }
	
	bool isVisibleTile(int _x, int _y, int _d);
	bool isVisibleTile(int _x, int _y) { return isVisibleTile(_x, _y, mCurrentDepth); }
	bool isVisibleTile(Tile _t) { return isVisibleTile(_t.x(), _t.y(), _t.depth()); }
	
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

	void initMapDrawParams();
	
	void draw();

	void serialize(NAS2D::Xml::XmlElement* _ti);
	void deserialize(NAS2D::Xml::XmlElement* _ti);

protected:
	enum MouseMapRegion
	{
		MMR_MIDDLE,
		MMR_TOP_RIGHT,
		MMR_TOP_LEFT,
		MMR_BOTTOM_RIGHT,
		MMR_BOTTOM_LEFT
	};
	
	std::vector<std::vector<MouseMapRegion> > mMouseMap;

private:
	typedef std::vector<std::vector<Tile> >	TileGrid;
	typedef std::vector<TileGrid>		TileArray;
	
private:

	// No default copy constructor, or copy operator
	// Calling these should result in an error
	TileMap(const TileMap&) = delete;
	TileMap& operator=(const TileMap&) = delete;

	void buildMouseMap();
	void buildTerrainMap(const std::string& path);
	void setupMines(int mineCount);

	void updateTileHighlight();

	MouseMapRegion getMouseMapRegion(int x, int y);

private:
	int					mEdgeLength;
	int					mWidth, mHeight;

	int					mMaxDepth;			/**< Maximum digging depth. */
	int					mCurrentDepth;		/**< Current depth level to view. */

	std::string			mMapPath;
	std::string			mTsetPath;

	TileArray			mTileMap;

	Image				mTileSelector;
	Image				mTileset;
	Image				mMineBeacon;

	Timer				mTimer;

	Point_2d			mMousePosition;		/**< Current mouse position. */
	Point_2d			mMapHighlight;		/**< Tile the mouse is pointing to. */
	Point_2d			mMapViewLocation;	/**<  */

	Point_2df			mMapPosition;		/** Where to start drawing the TileMap on the screen. */

	Point2dList			mMineLocations;		/**< Location of all mines on the map. */

	Rectangle_2d		mMapBoundingBox;	/** Area that the TileMap fills when drawn. */

	bool				mDebug;
	bool				mShowConnections;	/**< Flag indicating whether or not to highlight connectedness. */
};
