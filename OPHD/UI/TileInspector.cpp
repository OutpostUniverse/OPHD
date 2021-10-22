#include "TileInspector.h"

#include "TextRender.h"
#include "../Constants/Strings.h"
#include "../Mine.h"

#include <map>
#include <sstream>


using namespace NAS2D;


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

	auto position = mRect.startPoint() + NAS2D::Vector{5, 25};
	drawLabelAndValue(position, "Has Mine: ", (mine ? "Yes" : "No"));

	if (mine)
	{
		position.y += 10;
		drawLabelAndValue(position, "Active: ", (mine->active() ? "Yes" : "No"));

		position.y += 10;
		drawLabelAndValue(position, "Production Rate: ", MINE_YIELD_TRANSLATION.at(mTile->mine()->productionRate()));
	}

	position = mRect.startPoint() + NAS2D::Vector{5, 62};
	const auto tilePosition = mTile->xy();
	drawLabelAndValue(position, "Location: ", std::to_string(tilePosition.x) + ", " + std::to_string(tilePosition.y));

	position.y += 10;
	drawLabelAndValue(position, "Terrain: ", TILE_INDEX_TRANSLATION.at(mTile->index()));
}


void TileInspector::onClose()
{
	visible(false);
}

