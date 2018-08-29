// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TileInspector.h"

#include "../Constants.h"

#include <map>
#include <sstream>




TileInspector::TileInspector(Font& font):	mBold("fonts/opensans-bold.ttf", 10),
											mTile(nullptr)
{
	Control::font(font);
	text(constants::WINDOW_TILE_INSPECTOR);
	init();
}


TileInspector::~TileInspector()
{
}


void TileInspector::init()
{
	position(0, 0);
	size(200, 88);

	add(&btnClose, 145, 63);
	btnClose.font(font());
	btnClose.text("Close");
	btnClose.size(50, 20);
	btnClose.click().connect(this, &TileInspector::btnCloseClicked);
}


void TileInspector::update()
{
	if(!visible())
		return;

	if(!mTile)
		return;

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawText(mBold, "Has Mine:", rect().x() + 5, rect().y() + 25, 255, 255, 255);

	if(mTile->mine())
	{
		r.drawText(font(), "Yes", rect().x() + 5 + mBold.width("Has Mine: "), rect().y() + 25, 255, 255, 255);

		r.drawText(mBold, "Active:", rect().x() + 5, rect().y() + 35, 255, 255, 255);

		if (mTile->mine()->active())
		{
			r.drawText(font(), "Yes", rect().x() + 5 + mBold.width("Active: "), rect().y() + 35, 255, 255, 255);
		}
		else
		{
			r.drawText(font(), "No", rect().x() + 5 + mBold.width("Active: "), rect().y() + 35, 255, 255, 255);
		}

		r.drawText(mBold, "Production Rate:", rect().x() + 5, rect().y() + 45, 255, 255, 255);
		r.drawText(font(), MINE_YIELD_TRANSLATION[mTile->mine()->productionRate()], rect().x() + 5 + mBold.width("Production Rate: "), rect().y() + 45, 255, 255, 255);
	}
	else
	{
		r.drawText(font(), "No", rect().x() + 5 + mBold.width("Has Mine: "), rect().y() + 25, 255, 255, 255);
	}

	r.drawText(mBold, "Location:", rect().x() + 5, rect().y() + 62, 255, 255, 255);
	r.drawText(font(), string_format("%i, %i", mTile->x(), mTile->y()), rect().x() + 5 + mBold.width("Location: "), rect().y() + 62, 255, 255, 255);

	r.drawText(mBold, "Terrain:", rect().x() + 5, rect().y() + 72, 255, 255, 255);
	r.drawText(font(), TILE_INDEX_TRANSLATION[mTile->index()], rect().x() + 5 + mBold.width("Terrain: "), rect().y() + 72, 255, 255, 255);
}


void TileInspector::btnCloseClicked()
{
	visible(false);
}

