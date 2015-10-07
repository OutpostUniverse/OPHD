#include "TubesPalette.h"



TubesPalette::TubesPalette(Font& font)
{
	Control::font(font);
	init();
}


TubesPalette::~TubesPalette()
{
}


void TubesPalette::init()
{
	position(0, 0);
	size(112, 64);

	addControl("btnTubesIntersection", &btnTubesIntersection, 5, 5);
	btnTubesIntersection.image("ui/structures/tube_intersection.png");
	btnTubesIntersection.size(32, 32);
	btnTubesIntersection.click().Connect(this, &TubesPalette::btnTubesIntersectionClicked);
	btnTubesIntersection.type(Button::BUTTON_TOGGLE);

	addControl("btnTubesRight", &btnTubesRight, 39, 5);
	btnTubesRight.image("ui/structures/tubes_right.png");
	btnTubesRight.size(32, 32);
	btnTubesRight.click().Connect(this, &TubesPalette::btnTubesRightClicked);
	btnTubesRight.type(Button::BUTTON_TOGGLE);

	addControl("btnTubesLeft", &btnTubesLeft, 73, 5);
	btnTubesLeft.image("ui/structures/tubes_left.png");
	btnTubesLeft.size(32, 32);
	btnTubesLeft.click().Connect(this, &TubesPalette::btnTubesLeftClicked);
	btnTubesLeft.type(Button::BUTTON_TOGGLE);

	addControl("btnCancel", &btnCancel, 31, 40);
	btnCancel.font(font());
	btnCancel.text("Cancel");
	btnCancel.size(50, 20);
	btnCancel.click().Connect(this, &TubesPalette::btnCancelClicked);
}


void TubesPalette::onMouseDown(MouseButton button, int x, int y)
{
	if(!visible())
		return;
}


void TubesPalette::onMouseUp(MouseButton button, int x, int y)
{
	if(!visible())
		return;
}


void TubesPalette::update()
{
	if(!visible())
		return;

	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(rect(), COLOR_SILVER.red(), COLOR_SILVER.green(), COLOR_SILVER.blue());
	r.drawBox(rect(), 0, 0, 0);

	r.drawLine(rect().x() + 1, rect().y() + 1, rect().x() + rect().w() - 1, rect().y() + 1, COLOR_WHITE);
	r.drawLine(rect().x() + 1, rect().y() + 1, rect().x() + 1, rect().y() + rect().h() - 0.5f, COLOR_WHITE);

	r.drawLine(rect().x() + 2, rect().y() + rect().h() - 1, rect().x() + rect().w() - 2, rect().y() + rect().h() - 1, COLOR_GREY);
	r.drawLine(rect().x() + rect().w() - 1, rect().y() + 2, rect().x() + rect().w() - 1, rect().y() + rect().h() - 0.5f, COLOR_GREY);
	
	// Let UIContainer handle the basics.
	UIContainer::update();
}


void TubesPalette::clearToggles()
{
	btnTubesIntersection.toggle(false);
	btnTubesRight.toggle(false);
	btnTubesLeft.toggle(false);
}

void TubesPalette::btnCancelClicked()
{
	visible(false);
	mCallback(CONNECTOR_NONE, false);
}


void TubesPalette::btnTubesIntersectionClicked()
{
	btnTubesRight.toggle(false);
	btnTubesLeft.toggle(false);

	mCallback(CONNECTOR_INTERSECTION, btnTubesIntersection.toggled());
}


void TubesPalette::btnTubesRightClicked()
{
	btnTubesIntersection.toggle(false);
	btnTubesLeft.toggle(false);

	mCallback(CONNECTOR_RIGHT, btnTubesRight.toggled());
}


void TubesPalette::btnTubesLeftClicked()
{
	btnTubesRight.toggle(false);
	btnTubesIntersection.toggle(false);

	mCallback(CONNECTOR_LEFT, btnTubesLeft.toggled());
}
