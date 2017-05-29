#include "TileInspector.h"

#include "../Constants.h"

#include <map>
#include <sstream>

std::map<int, std::string> TileIndexTranslation;
std::map<Mine::ProductionRate, std::string> MineProductionTranslation;


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

	addControl("btnClose", &btnClose, 145, 63);
	btnClose.font(font());
	btnClose.text("Close");
	btnClose.size(50, 20);
	btnClose.click().connect(this, &TileInspector::btnCloseClicked);

	// Build translation tables.
	TileIndexTranslation[0] = "Bulldozed";
	TileIndexTranslation[1] = "Clear";
	TileIndexTranslation[2] = "Rough";
	TileIndexTranslation[3] = "Difficult";
	TileIndexTranslation[4] = "Impassable";

	MineProductionTranslation[Mine::ProductionRate::PRODUCTION_RATE_HIGH] = "High";
	MineProductionTranslation[Mine::ProductionRate::PRODUCTION_RATE_LOW] = "Low";
	MineProductionTranslation[Mine::ProductionRate::PRODUCTION_RATE_MEDIUM] = "Medium";
}


void TileInspector::update()
{
	if(!visible())
		return;

	if(!mTile)
		return;

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawText(mBold, "Has Mine:", rect().x() + 5, rect().y() + 23, 255, 255, 255);

	if(mTile->mine())
	{
		r.drawText(font(), "Yes", rect().x() + 5 + mBold.width("Has Mine: "), rect().y() + 23, 255, 255, 255);

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
		r.drawText(font(), MineProductionTranslation[mTile->mine()->productionRate()], rect().x() + 5 + mBold.width("Production Rate: "), rect().y() + 45, 255, 255, 255);
	}
	else
	{
		r.drawText(font(), "No", rect().x() + 5 + mBold.width("Has Mine: "), rect().y() + 25, 255, 255, 255);
	}

	r.drawText(mBold, "Location:", rect().x() + 5, rect().y() + 62, 255, 255, 255);
	r.drawText(font(), string_format("%i, %i", mTile->x(), mTile->y()), rect().x() + 5 + mBold.width("Location: "), rect().y() + 62, 255, 255, 255);

	r.drawText(mBold, "Terrain:", rect().x() + 5, rect().y() + 72, 255, 255, 255);
	r.drawText(font(), TileIndexTranslation[mTile->index()], rect().x() + 5 + mBold.width("Terrain: "), rect().y() + 72, 255, 255, 255);
}


void TileInspector::btnCloseClicked()
{
	visible(false);
}

