#include "TileInspector.h"

#include <map>
#include <sstream>

extern std::stringstream str_scratch; // FIXME: Ugly hack

std::map<int, std::string> TileIndexTranslation;
std::map<Mine::ProductionRate, std::string> MineProductionTranslation;

TileInspector::TileInspector(Font& font):	mBold("fonts/ui-bold.png", 7, 9, 0),
											mTile(nullptr)
{
	Control::font(font);
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
	btnClose.click().Connect(this, &TileInspector::btnCloseClicked);

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


void TileInspector::onMouseDown(MouseButton button, int x, int y)
{
	if(!visible())
		return;
}


void TileInspector::onMouseUp(MouseButton button, int x, int y)
{
	if(!visible())
		return;
}


void TileInspector::update()
{
	if(!visible())
		return;

	if(!mTile)
		return;

	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(rect(), COLOR_SILVER.red(), COLOR_SILVER.green(), COLOR_SILVER.blue());
	r.drawBoxFilled(rect().x(), rect().y(), rect().w(), 19.0f, COLOR_SILVER.red() + 25, COLOR_SILVER.green() + 25, COLOR_SILVER.blue() + 25);
	r.drawBox(rect(), 0, 0, 0);

	r.drawText(mBold, "Tile Inspector", rect().x() + 5, rect().y() + 5, 0, 0, 0);

	r.drawText(mBold, "Has Mine:", rect().x() + 5, rect().y() + 25, 0, 0, 0);

	if(mTile->mine())
	{
		r.drawText(font(), "Yes", rect().x() + 5 + mBold.width("Has Mine: "), rect().y() + 25, 0, 0, 0);

		r.drawText(mBold, "Active:", rect().x() + 5, rect().y() + 35, 0, 0, 0);

		if (mTile->mine()->active())
		{
			r.drawText(font(), "Yes", rect().x() + 5 + mBold.width("Active: "), rect().y() + 35, 0, 0, 0);
		}
		else
		{
			r.drawText(font(), "No", rect().x() + 5 + mBold.width("Active: "), rect().y() + 35, 0, 0, 0);
		}

		r.drawText(mBold, "Production Rate:", rect().x() + 5, rect().y() + 45, 0, 0, 0);
		r.drawText(font(), MineProductionTranslation[mTile->mine()->productionRate()], rect().x() + 5 + mBold.width("Production Rate: "), rect().y() + 45, 0, 0, 0);
	}
	else
	{
		r.drawText(font(), "No", rect().x() + 5 + mBold.width("Has Mine: "), rect().y() + 25, 0, 0, 0);
	}

	str_scratch.str("");
	str_scratch << mTile->x() << ", " << mTile->y();
	r.drawText(font(), str_scratch.str(), rect().x() + 5 + mBold.width("Location: "), rect().y() + 65, 0, 0, 0);
	r.drawText(mBold, "Location:", rect().x() + 5, rect().y() + 65, 0, 0, 0);

	str_scratch.str("");
	str_scratch << TileIndexTranslation[mTile->index()];
	r.drawText(font(), str_scratch.str(), rect().x() + 5 + mBold.width("Terrain: "), rect().y() + 75, 0, 0, 0);
	r.drawText(mBold, "Terrain:", rect().x() + 5, rect().y() + 75, 0, 0, 0);


	// Let UIContainer handle the basics.
	UIContainer::update();
}


void TileInspector::btnCloseClicked()
{
	visible(false);
}

