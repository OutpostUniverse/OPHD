#include "TileInspector.h"

#include "TextRender.h"
#include "../MineProductionRateString.h"
#include "../Constants/Strings.h"
#include "../MapObjects/Mine.h"

#include <libOPHD/EnumTerrainType.h>

#include <map>
#include <sstream>


using namespace NAS2D;


namespace
{
	const std::map<TerrainType, std::string> terrainTypeStringTable =
	{
		{TerrainType::Dozed, constants::TileBulldozed},
		{TerrainType::Clear, constants::TileClear},
		{TerrainType::Rough, constants::TileRough},
		{TerrainType::Difficult, constants::TileDifficult},
		{TerrainType::Impassable, constants::TileImpassable},
	};
}


TileInspector::TileInspector() :
	Window{constants::WindowTileInspector},
	btnClose{"Close", {this, &TileInspector::onClose}}
{
	size({200, 88});

	add(btnClose, {145, 63});
	btnClose.size({50, 20});
}


void TileInspector::update()
{
	if (!visible())
		return;

	if (!mTile)
		return;

	Window::update();

	const auto* mine = mTile->mine();

	auto position = mRect.position + NAS2D::Vector{5, 25};
	drawLabelAndValue(position, "Has Mine: ", (mine ? "Yes" : "No"));

	if (mine)
	{
		position.y += 10;
		drawLabelAndValue(position, "Active: ", (mine->active() ? "Yes" : "No"));

		position.y += 10;
		drawLabelAndValue(position, "Production Rate: ", mineProductionRateEnumToString(mTile->mine()->productionRate()));
	}

	position = mRect.position + NAS2D::Vector{5, 62};
	const auto tilePosition = mTile->xy();
	drawLabelAndValue(position, "Location: ", std::string{tilePosition});

	position.y += 10;
	drawLabelAndValue(position, "Terrain: ", terrainTypeStringTable.at(mTile->index()));
}


void TileInspector::onClose()
{
	visible(false);
}

