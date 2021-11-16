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


NAS2D::Rectangle<int> MapView::viewArea() const
{
	return {mOriginTilePosition.xy.x, mOriginTilePosition.xy.y, mEdgeLength, mEdgeLength};
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


void MapView::moveView(Direction direction)
{
	mapViewLocation({
		mOriginTilePosition.xy + directionEnumToOffset(direction),
		mOriginTilePosition.z + directionEnumToVerticalOffset(direction)
	});
}


void MapView::currentDepth(int i)
{
	mOriginTilePosition.z = std::clamp(i, 0, mTileMap.maxDepth());
}


int MapView::viewSize() const
{
	return mEdgeLength;
}


void MapView::viewSize(int sizeInTiles)
{
	mEdgeLength = std::max(3, sizeInTiles);
	mapViewLocation(mOriginTilePosition);
}


bool MapView::isVisibleTile(const MapCoordinate& position) const
{
	return viewArea().contains(position.xy) && position.z == mOriginTilePosition.z;
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
