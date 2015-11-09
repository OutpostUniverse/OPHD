#include "TileInspector.h"



TileInspector::TileInspector(Font& font):	mTile(NULL)
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
	size(200, 100);

	addControl("btnClose", &btnClose, 145, 75);
	btnClose.font(font());
	btnClose.text("Close");
	btnClose.size(50, 20);
	btnClose.click().Connect(this, &TileInspector::btnCloseClicked);

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
	r.drawBox(rect(), 0, 0, 0);


	/**
	 * Draw some information about the selected tile.
	 * 
	 * I know, this is awful. I'll clean it up at some point in the future, I'm just
	 * lazy right now.
	 */
	if(mTile->mine())
	{
		r.drawText(font(), "Has Mine: YES", rect().x() + 5, rect().y() + 5, 0, 0, 0);

		if(mTile->mine()->active())
			r.drawText(font(), "Active: YES", rect().x() + 5, rect().y() + 15, 0, 0, 0);
		else
			r.drawText(font(), "Active: NO", rect().x() + 5, rect().y() + 15, 0, 0, 0);

		if(mTile->mine()->productionRate() == Mine::PRODUCTION_RATE_HIGH)
			r.drawText(font(), "Production Rate: HIGH", rect().x() + 5, rect().y() + 25, 0, 0, 0);
		else if(mTile->mine()->productionRate() == Mine::PRODUCTION_RATE_MEDIUM)
			r.drawText(font(), "Production Rate: MEDIUM", rect().x() + 5, rect().y() + 25, 0, 0, 0);
		else
			r.drawText(font(), "Production Rate: LOW", rect().x() + 5, rect().y() + 25, 0, 0, 0);
	}
	else
	{
		r.drawText(font(), "Has Mine: NO", rect().x() + 5, rect().y() + 5, 0, 0, 0);
	}


	// Let UIContainer handle the basics.
	UIContainer::update();
}


void TileInspector::btnCloseClicked()
{
	visible(false);
}

