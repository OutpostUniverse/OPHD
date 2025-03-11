#include "TileInspector.h"

#include "TextRender.h"
#include "../MineProductionRateString.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/OreDeposit.h"

#include <libOPHD/EnumTerrainType.h>

#include <NAS2D/StringFrom.h>

#include <map>
#include <sstream>


using namespace NAS2D;


namespace
{
	const auto lineSpacing = 12;
	const auto sectionSpacing = constants::Margin;

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
	size({200, sWindowTitleBarHeight + lineSpacing * 5 + sectionSpacing + constants::Margin * 2});

	btnClose.size({50, 20});
	add(btnClose, size() - btnClose.size() - Vector{constants::Margin, constants::Margin});
}


void TileInspector::update()
{
	if (!visible())
		return;

	if (!mTile)
		return;

	if(!mTile->excavated())
	{
		visible(false);
		return;
	}

	Window::update();

	auto position = mRect.position + NAS2D::Vector{5, 25};
	const auto tilePosition = mTile->xy();
	drawLabelAndValue(position, "Location: ", NAS2D::stringFrom(tilePosition));

	position.y += lineSpacing;
	drawLabelAndValue(position, "Terrain: ", terrainTypeStringTable.at(mTile->index()));

	const auto* mine = mTile->mine();

	position.y += lineSpacing + sectionSpacing;
	drawLabelAndValue(position, "Has Mine: ", (mine ? "Yes" : "No"));

	if (mine)
	{
		position.y += lineSpacing;
		drawLabelAndValue(position, "Active: ", (mine->active() ? "Yes" : "No"));

		position.y += lineSpacing;
		drawLabelAndValue(position, "Production Rate: ", mineProductionRateEnumToString(mTile->mine()->productionRate()));
	}
}


void TileInspector::onClose()
{
	visible(false);
}

