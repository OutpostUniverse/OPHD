#include "TileInspector.h"

#include "TextRender.h"
#include "../OreDepositYieldString.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include <libOPHD/EnumTerrainType.h>
#include <libOPHD/MapObjects/OreDeposit.h>

#include <NAS2D/StringFrom.h>

#include <map>
#include <sstream>


using namespace NAS2D;


namespace
{
	const auto lineSpacing = 18;
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


void TileInspector::drawClientArea(NAS2D::Renderer& /*renderer*/) const
{
	auto position = mRect.position + NAS2D::Vector{5, 25};
	const auto tilePosition = mTile->xy();
	drawLabelAndValue(position, "Location: ", NAS2D::stringFrom(tilePosition));

	position.y += lineSpacing;
	drawLabelAndValue(position, "Terrain: ", terrainTypeStringTable.at(mTile->index()));

	const auto* oreDeposit = mTile->oreDeposit();

	position.y += lineSpacing + sectionSpacing;
	drawLabelAndValue(position, "Has Ore Deposit: ", (oreDeposit ? "Yes" : "No"));

	if (oreDeposit)
	{
		position.y += lineSpacing;
		drawLabelAndValue(position, "Yield: ", oreDepositYieldEnumToString(mTile->oreDeposit()->yield()));
	}
}


void TileInspector::onVisibilityChange(bool isVisible)
{
	Window::onVisibilityChange(isVisible);

	if (isVisible && (!mTile || !mTile->excavated()))
	{
		visible(false);
	}
}


void TileInspector::onClose()
{
	visible(false);
}

