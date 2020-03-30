// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TileInspector.h"

#include "../Constants.h"
#include "../FontManager.h"

#include <map>
#include <sstream>

using namespace NAS2D;

static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;


TileInspector::TileInspector() :
	btnClose{"Close"}
{
	text(constants::WINDOW_TILE_INSPECTOR);
	init();
}


TileInspector::~TileInspector()
{
}


void TileInspector::init()
{
	size(200, 88);

	add(&btnClose, 145, 63);
	btnClose.size(50, 20);
	btnClose.click().connect(this, &TileInspector::btnCloseClicked);

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
}


void TileInspector::update()
{
	if(!visible())
		return;

	if(!mTile)
		return;

	Window::update();

	Renderer& renderer = Utility<Renderer>::get();

	const auto drawTitleText = [&renderer](NAS2D::Point<int> position, std::string title, std::string text) {
		renderer.drawText(*FONT_BOLD, title, position, NAS2D::Color::White);
		position.x() += FONT_BOLD->width(title);
		renderer.drawText(*FONT, text, position, NAS2D::Color::White);
	};

	const auto* mine = mTile->mine();

	auto position = rect().startPoint() + NAS2D::Vector{5, 25};
	drawTitleText(position, "Has Mine: ", (mine ? "Yes" : "No"));

	if(mine)
	{
		position.y() += 10;
		drawTitleText(position, "Active: ", (mine->active() ? "Yes" : "No"));

		position.y() += 10;
		drawTitleText(position, "Production Rate: ", MINE_YIELD_TRANSLATION[mTile->mine()->productionRate()]);
	}

	position = rect().startPoint() + NAS2D::Vector{5, 62};
	drawTitleText(position, "Location: ", std::to_string(mTile->x()) + ", " + std::to_string(mTile->y()));

	position.y() += 10;
	drawTitleText(position, "Terrain: ", TILE_INDEX_TRANSLATION[mTile->index()]);
}


void TileInspector::btnCloseClicked()
{
	visible(false);
}

