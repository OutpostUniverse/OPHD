#pragma once

#include "Tile.h"

#include "../Things/Structures/Structure.h"
#include "../MicroPather/micropather.h"

#include "NAS2D/Renderer/Point.h"
#include "NAS2D/Renderer/Vector.h"

#include <algorithm>

using Point2dList = std::vector<NAS2D::Point_2d>;

class TileMap: public micropather::Graph
{
public:
	/**
	 * 
	 */
	enum TileMapLevel
	{
		LEVEL_SURFACE = 0,
		LEVEL_UG_1,
		LEVEL_UG_2,
		LEVEL_UG_3,
		LEVEL_UG_4
	};

public:
	TileMap(const std::string& map_path, const std::string& tset_path, int maxDepth, int mineCount, constants::PlanetHostility hostility /*= constants::HOSTILITY_NONE*/, bool setupMines = true);
	~TileMap();

	Tile* getTile(int x, int y, int level);
	Tile* getTile(int x, int y) { return getTile(x, y, mCurrentDepth); }
	Tile* getTile(NAS2D::Point<int> position, int level) { return getTile(position.x(), position.y(), level); }

	Tile* getVisibleTile(NAS2D::Point<int> position, int level);
	Tile* getVisibleTile() { return getVisibleTile(tileMouseHover(), mCurrentDepth); }

	bool isVisibleTile(NAS2D::Point<int> position, int z) const;
	bool isVisibleTile(NAS2D::Point<int> position) const { return isVisibleTile(position, mCurrentDepth); }
	bool isVisibleTile(const Tile& t) { return isVisibleTile(t.position(), t.depth()); }

	const NAS2D::Rectangle_2d& boundingBox() const { return mMapBoundingBox; }

	const NAS2D::Point_2d& mapViewLocation() const { return mMapViewLocation; }
	void mapViewLocation(int x, int y) { mMapViewLocation = {x, y}; }
	void centerMapOnTile(Tile*);

	const NAS2D::Point_2d& tileHighlight() const { return mMapHighlight; }
	bool tileHighlightVisible() const;

	int tileMouseHoverX() const { return mMapHighlight.x() + mMapViewLocation.x(); }
	int tileMouseHoverY() const { return mMapHighlight.y() + mMapViewLocation.y(); }
	NAS2D::Point_2d tileMouseHover() const { return NAS2D::Point_2d(tileMouseHoverX(), tileMouseHoverY()); }

	const Point2dList& mineLocations() const { return mMineLocations; }
	void removeMineLocation(const NAS2D::Point_2d& pt);

	void toggleShowConnections() { mShowConnections = !mShowConnections; }

	int edgeLength() const { return mEdgeLength; }
	int width() const { return mWidth; }
	int height() const { return mHeight; }
	NAS2D::Vector<int> size() const { return {mWidth, mHeight}; }

	int currentDepth() const { return mCurrentDepth; }
	void currentDepth(int i) { mCurrentDepth = std::clamp(i, 0, mMaxDepth); }

	int maxDepth() const { return mMaxDepth; }

	void injectMouse(int x, int y);
	void injectMouse(NAS2D::Point<int> position) { mMousePosition = position; }

	void initMapDrawParams(int, int);
	
	void draw();

	void serialize(NAS2D::Xml::XmlElement* element);
	void deserialize(NAS2D::Xml::XmlElement* element);

public:
	/** MicroPather public interface implementation. */
	virtual float LeastCostEstimate(void* stateStart, void* stateEnd);
	virtual void AdjacentCost(void* state, std::vector<micropather::StateCost>* adjacent);
	virtual void PrintStateInfo(void* /*state*/) {};

protected:
	/**
	 * 
	 */
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
	using TileGrid = std::vector<std::vector<Tile> >;
	using TileArray = std::vector<TileGrid>;
	
private:
	TileMap(const TileMap&) = delete;						/**< Not Allowed */
	TileMap& operator=(const TileMap&) = delete;			/**< Not allowed */

private:
	void buildMouseMap();
	void buildTerrainMap(const std::string& path);
	void setupMines(int, constants::PlanetHostility);

	void updateTileHighlight();

	MouseMapRegion getMouseMapRegion(int x, int y);

private:
	int					mEdgeLength = 0;			/**<  */
	int					mWidth = 0;					/**<  */
	int					mHeight = 0;				/**<  */

	int					mMaxDepth = 0;				/**< Maximum digging depth. */
	int					mCurrentDepth = 0;			/**< Current depth level to view. */

	std::string			mMapPath;					/**<  */
	std::string			mTsetPath;					/**<  */

	TileArray			mTileMap;					/**<  */

	NAS2D::Image		mTileset;					/**<  */
	NAS2D::Image		mMineBeacon;				/**<  */

	NAS2D::Timer		mTimer;						/**<  */

	NAS2D::Point_2d		mMousePosition;				/**< Current mouse position. */
	NAS2D::Point_2d		mMapHighlight;				/**< Tile the mouse is pointing to. */
	NAS2D::Point_2d		mMapViewLocation;			/**<  */

	NAS2D::Point_2df	mMapPosition;				/** Where to start drawing the TileMap on the screen. */

	Point2dList			mMineLocations;				/**< Location of all mines on the map. */

	NAS2D::Rectangle_2d	mMapBoundingBox;			/** Area that the TileMap fills when drawn. */

	bool				mShowConnections = false;	/**< Flag indicating whether or not to highlight connectedness. */
};
