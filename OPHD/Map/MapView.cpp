#include "MapView.h"

#include "TileMap.h"
#include "../DirectionOffset.h"

#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlElement.h>

#include <algorithm>


MapView::MapView(TileMap& tileMap) :
	mTileMap{tileMap}
{}


Tile& MapView::getTile(NAS2D::Point<int> position)
{
	return mTileMap.getTile({position, mOriginTilePosition.z});
}


NAS2D::Rectangle<int> MapView::viewTileRect() const
{
	return {mOriginTilePosition.xy, {mEdgeLength, mEdgeLength}};
}


void MapView::mapViewLocation(const MapCoordinate& position)
{
	const auto sizeInTiles = mTileMap.size();
	mOriginTilePosition.xy = {
		std::clamp(position.xy.x, 0, sizeInTiles.x - mEdgeLength),
		std::clamp(position.xy.y, 0, sizeInTiles.y - mEdgeLength)
	};
	currentDepth(position.z);
}


void MapView::centerOn(NAS2D::Point<int> point)
{
	centerOn({point, mOriginTilePosition.z});
}


void MapView::centerOn(const MapCoordinate& position)
{
	mapViewLocation({position.xy - NAS2D::Vector{mEdgeLength, mEdgeLength} / 2, position.z});
}


void MapView::moveView(MapOffset direction)
{
	mapViewLocation(mOriginTilePosition.translate(direction));
}


void MapView::moveView(Direction direction)
{
	mapViewLocation(mOriginTilePosition.translate(direction));
}


void MapView::currentDepth(int i)
{
	mOriginTilePosition.z = std::clamp(i, 0, maxDepth());
}


bool MapView::isSurface() const
{
	return mOriginTilePosition.z == 0;
}


int MapView::maxDepth() const
{
	return mTileMap.maxDepth();
}


int MapView::viewSize() const
{
	return mEdgeLength;
}


void MapView::viewSize(int edgeSizeInTiles)
{
	// Set a cap on view size at the smallest map dimension
	// The view diamond needs to be square, and we don't want to overflow map bounds when drawing the square
	const auto sizeInTiles = mTileMap.size();
	const auto maxViewSize = std::min(sizeInTiles.x, sizeInTiles.y);
	mEdgeLength = std::clamp(edgeSizeInTiles, 3, maxViewSize);
	// Re-clamp view location based on new edge length
	mapViewLocation(mOriginTilePosition);
}


bool MapView::isVisibleTile(const MapCoordinate& position) const
{
	return viewTileRect().contains(position.xy) && position.z == mOriginTilePosition.z;
}


void MapView::serialize(NAS2D::Xml::XmlElement* element)
{
	// ==========================================
	// VIEW PARAMETERS
	// ==========================================
	element->linkEndChild(NAS2D::dictionaryToAttributes(
		"view_parameters",
		{{
			{"currentdepth", mOriginTilePosition.z},
			{"viewlocation_x", mOriginTilePosition.xy.x},
			{"viewlocation_y", mOriginTilePosition.xy.y},
		}}
	));
}


void MapView::deserialize(NAS2D::Xml::XmlElement* element)
{
	// VIEW PARAMETERS
	auto* viewParameters = element->firstChildElement("view_parameters");
	const auto dictionary = NAS2D::attributesToDictionary(*viewParameters);

	const auto viewX = dictionary.get<int>("viewlocation_x");
	const auto viewY = dictionary.get<int>("viewlocation_y");
	const auto viewDepth = dictionary.get<int>("currentdepth");

	mapViewLocation({{viewX, viewY}, viewDepth});
}
