#pragma once

#include "../Map/MapCoordinate.h"

#include <NAS2D/Math/Rectangle.h>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}

class Tile;
class TileMap;


class MapView
{
public:
	MapView(TileMap& tileMap);

	Tile& getTile(NAS2D::Point<int> position);

	NAS2D::Rectangle<int> viewTileRect() const;
	void mapViewLocation(const MapCoordinate& position);
	void centerOn(NAS2D::Point<int> point);
	void centerOn(const MapCoordinate& position);
	void moveView(Direction direction);

	int currentDepth() const { return mOriginTilePosition.z; }
	void currentDepth(int i);

	int viewSize() const;
	void viewSize(int edgeSizeInTiles);

	bool isVisibleTile(const MapCoordinate& position) const;

	void serialize(NAS2D::Xml::XmlElement* element);
	void deserialize(NAS2D::Xml::XmlElement* element);

private:
	TileMap& mTileMap;
	int mEdgeLength = 0;
	MapCoordinate mOriginTilePosition{{0, 0}, 0}; // Top tile of detail view diamond, or top left corner of minimap view box
};
