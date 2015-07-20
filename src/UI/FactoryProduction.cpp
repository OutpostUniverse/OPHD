#include "FactoryProduction.h"

#include "../UiConstants.h"


FactoryProduction::FactoryProduction(Font& font) : mFactory(nullptr)
{
	Control::font(font);
	init();
}


FactoryProduction::~FactoryProduction()
{}


void FactoryProduction::update()
{
	if (!visible())
		return;

	Utility<Renderer>::get().drawBoxFilled(rect(), COLOR_SILVER.red(), COLOR_SILVER.green(), COLOR_SILVER.blue());
	Utility<Renderer>::get().drawBox(rect(), 0, 0, 0);

	// FIXME: Not efficient.
	stringstream ss;
	ss << "Factory Production ID(" << mFactory->id() << ")";
	Utility<Renderer>::get().drawText(font(), ss.str(), rect().x() + 5, rect().y() + 5, 0, 0, 0);
	Utility<Renderer>::get().drawText(font(), ss.str(), rect().x() + 5.5, rect().y() + 5.5, 0, 0, 0);


	Utility<Renderer>::get().drawBoxFilled(mnuProductionList.rect(), 0, 0, 0);

	// Let UIContainer handle the basics.
	UIContainer::update();
}


void FactoryProduction::init()
{
	size(380, 115);
	position(static_cast<int>(Utility<Renderer>::get().screenCenterX() - width() / 2), static_cast<int>((Utility<Renderer>::get().height() - constants::BOTTOM_UI_HEIGHT) / 2 - height() / 2));

	addControl("mnuProductionList", &mnuProductionList, 5, 25);
	mnuProductionList.font(font());
	mnuProductionList.width(195);

	mnuProductionList.addItem("Production 1");
	mnuProductionList.addItem("Production 2");
	mnuProductionList.addItem("Production 3");
	mnuProductionList.addItem("Production 4");
	mnuProductionList.addItem("Production 5");
	mnuProductionList.addItem("Production 6");

	mnuProductionList.currentSelection(0);

	addControl("btnIdle", &btnIdle, 205, 92);
	btnIdle.font(font());
	btnIdle.text("Idle");
	btnIdle.size(35, 18);
	btnIdle.click().Connect(this, &FactoryProduction::btnIdleClicked);
	btnIdle.type(Button::BUTTON_TOGGLE);

	addControl("btnOkay", &btnOkay, 260, 92);
	btnOkay.font(font());
	btnOkay.text("Okay");
	btnOkay.size(35, 18);
	btnOkay.click().Connect(this, &FactoryProduction::btnOkayClicked);

	addControl("btnCancel", &btnCancel, 300, 92);
	btnCancel.font(font());
	btnCancel.text("Cancel");
	btnCancel.size(35, 18);
	btnCancel.click().Connect(this, &FactoryProduction::btnCancelClicked);

	addControl("btnApply", &btnApply, 340, 92);
	btnApply.font(font());
	btnApply.text("Apply");
	btnApply.size(35, 18);
	btnApply.click().Connect(this, &FactoryProduction::btnApplyClicked);
}


void FactoryProduction::onMouseDown(MouseButton button, int x, int y)
{
	if (!visible())
		return;
}


void FactoryProduction::onMouseUp(MouseButton button, int x, int y)
{
	if (!visible())
		return;
}


void FactoryProduction::btnIdleClicked()
{
}


void FactoryProduction::btnOkayClicked()
{
	//hide();
	//mFactory = nullptr;
}


void FactoryProduction::btnApplyClicked()
{
}


void FactoryProduction::btnCancelClicked()
{
	//hide();
	//mFactory = nullptr;
}
