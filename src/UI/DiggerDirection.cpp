#include "DiggerDirection.h"



DiggerDirection::DiggerDirection(Font& font)
{
	Control::font(font);
	text("Direction");
	init();
}


DiggerDirection::~DiggerDirection()
{}


void DiggerDirection::init()
{
	position(0, 0);
	size(74, 170);

	// create a set of buttons
	addControl("btnDown", &btnDown, 5, 25);
	btnDown.image("ui/icons/arrow-down.png");
	btnDown.size(64, 34);
	btnDown.click().Connect(this, &DiggerDirection::btnDiggerDownClicked);

	addControl("btnWest", &btnWest, 5, 68);
	btnWest.image("ui/icons/arrow-west.png");
	btnWest.size(32, 32);
	btnWest.click().Connect(this, &DiggerDirection::btnDiggerWestClicked);

	addControl("btnNorth", &btnNorth, 38, 68);
	btnNorth.image("ui/icons/arrow-north.png");
	btnNorth.size(32, 32);
	btnNorth.click().Connect(this, &DiggerDirection::btnDiggerNorthClicked);

	addControl("btnSouth", &btnSouth, 5, 101);
	btnSouth.image("ui/icons/arrow-south.png");
	btnSouth.size(32, 32);
	btnSouth.click().Connect(this, &DiggerDirection::btnDiggerSouthClicked);

	addControl("btnEast", &btnEast, 38, 101);
	btnEast.image("ui/icons/arrow-east.png");
	btnEast.size(32, 32);
	btnEast.click().Connect(this, &DiggerDirection::btnDiggerEastClicked);

	addControl("btnCancel", &btnCancel, 5, 140);
	btnCancel.font(font());
	btnCancel.text("Cancel");
	btnCancel.size(64, 25);
	btnCancel.click().Connect(this, &DiggerDirection::btnCancelClicked);
}


void DiggerDirection::setParameters(Tile* _t)
{
	mTile = _t;
}


void DiggerDirection::downOnlyEnabled()
{
	btnDown.enabled(true);

	btnWest.enabled(false);
	btnNorth.enabled(false);
	btnSouth.enabled(false);
	btnEast.enabled(false);
}


void DiggerDirection::cardinalOnlyEnabled()
{
	btnDown.enabled(false);

	btnWest.enabled(true);
	btnNorth.enabled(true);
	btnSouth.enabled(true);
	btnEast.enabled(true);
}


void DiggerDirection::allEnabled()
{
	btnDown.enabled(true);

	btnWest.enabled(true);
	btnNorth.enabled(true);
	btnSouth.enabled(true);
	btnEast.enabled(true);
}

void DiggerDirection::btnCancelClicked()
{
	visible(false);
}


void DiggerDirection::update()
{
	if (!visible())
		return;

	if (!mTile)
		return;

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	/*
	r.drawBoxFilled(rect(), COLOR_SILVER.red(), COLOR_SILVER.green(), COLOR_SILVER.blue());
	r.drawBox(rect(), 0, 0, 0);

	r.drawLine(rect().x() + 1, rect().y() + 1, rect().x() + rect().w() - 1, rect().y() + 1, COLOR_WHITE);
	r.drawLine(rect().x() + 1, rect().y() + 1, rect().x() + 1, rect().y() + rect().h() - 0.5f, COLOR_WHITE);

	r.drawLine(rect().x() + 2, rect().y() + rect().h() - 1, rect().x() + rect().w() - 2, rect().y() + rect().h() - 1, COLOR_GREY);
	r.drawLine(rect().x() + rect().w() - 1, rect().y() + 2, rect().x() + rect().w() - 1, rect().y() + rect().h() - 0.5f, COLOR_GREY);
	*/
}


void DiggerDirection::selectDown()
{
	btnDiggerDownClicked();
}


void DiggerDirection::btnDiggerDownClicked()
{
	mCallback(SEL_DOWN, mTile);
}


void DiggerDirection::btnDiggerNorthClicked()
{
	mCallback(SEL_NORTH, mTile);
}


void DiggerDirection::btnDiggerSouthClicked()
{
	mCallback(SEL_SOUTH, mTile);
}


void DiggerDirection::btnDiggerEastClicked()
{
	mCallback(SEL_EAST, mTile);
}


void DiggerDirection::btnDiggerWestClicked()
{
	mCallback(SEL_WEST, mTile);
}