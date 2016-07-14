#include "StructureInspector.h"

#include <map>
#include <sstream>

extern std::stringstream str_scratch; // FIXME: Ugly hack


StructureInspector::StructureInspector(Font& font) :	mBold("fonts/ui-bold.png", 7, 9, 0),
														mStructure(nullptr)
{
	Control::font(font);
	init();
}


StructureInspector::~StructureInspector()
{
}


void StructureInspector::init()
{
	position(0, 0);
	size(350, 200);

	addControl("btnClose", &btnClose, 295, 175);
	btnClose.font(font());
	btnClose.text("Close");
	btnClose.size(50, 20);
	btnClose.click().Connect(this, &StructureInspector::btnCloseClicked);

	// Build translation tables.
}


void StructureInspector::onMouseDown(MouseButton button, int x, int y)
{
	if (!visible())
		return;
}


void StructureInspector::onMouseUp(MouseButton button, int x, int y)
{
	if (!visible())
		return;
}


void StructureInspector::update()
{
	if (!visible())
		return;

	if (!mStructure)
		return;

	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(rect(), COLOR_SILVER.red(), COLOR_SILVER.green(), COLOR_SILVER.blue());
	r.drawBoxFilled(rect().x(), rect().y(), rect().w(), 19.0f, COLOR_SILVER.red() + 25, COLOR_SILVER.green() + 25, COLOR_SILVER.blue() + 25);
	r.drawBox(rect(), 0, 0, 0);

	r.drawText(mBold, "Structure Inspector", rect().x() + 5, rect().y() + 5, 0, 0, 0);

	UIContainer::update();
}


void StructureInspector::btnCloseClicked()
{
	visible(false);
}

