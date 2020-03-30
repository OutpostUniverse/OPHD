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

	Renderer& r = Utility<Renderer>::get();

	const auto drawTitleText = [&renderer = r](NAS2D::Point<int> position, std::string title, std::string text) {
		renderer.drawText(*FONT_BOLD, title, position, NAS2D::Color::White);
		position.x() += FONT_BOLD->width(title);
		renderer.drawText(*FONT, text, position, NAS2D::Color::White);
	};

	const auto* mine = mTile->mine();

	auto position = rect().startPoint() + NAS2D::Vector{5, 25};
	drawTitleText(position, "Has Mine: ", (mine ? "Yes" : "No"));

	if(mTile->mine())
	{
		r.drawText(*FONT_BOLD, "Active:", rect().x() + 5, rect().y() + 35, 255, 255, 255);

		if (mTile->mine()->active())
		{
			r.drawText(*FONT, "Yes", rect().x() + 5 + FONT_BOLD->width("Active: "), rect().y() + 35, 255, 255, 255);
		}
		else
		{
			r.drawText(*FONT, "No", rect().x() + 5 + FONT_BOLD->width("Active: "), rect().y() + 35, 255, 255, 255);
		}

		r.drawText(*FONT_BOLD, "Production Rate:", rect().x() + 5, rect().y() + 45, 255, 255, 255);
		r.drawText(*FONT, MINE_YIELD_TRANSLATION[mTile->mine()->productionRate()], rect().x() + 5 + FONT_BOLD->width("Production Rate: "), rect().y() + 45, 255, 255, 255);
	}

	r.drawText(*FONT_BOLD, "Location:", rect().x() + 5, rect().y() + 62, 255, 255, 255);
	r.drawText(*FONT, string_format("%i, %i", mTile->x(), mTile->y()), rect().x() + 5 + FONT_BOLD->width("Location: "), rect().y() + 62, 255, 255, 255);

	r.drawText(*FONT_BOLD, "Terrain:", rect().x() + 5, rect().y() + 72, 255, 255, 255);
	r.drawText(*FONT, TILE_INDEX_TRANSLATION[mTile->index()], rect().x() + 5 + FONT_BOLD->width("Terrain: "), rect().y() + 72, 255, 255, 255);
}


void TileInspector::btnCloseClicked()
{
	visible(false);
}

