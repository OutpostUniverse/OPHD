// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MineOperationsWindow.h"

#include "../Constants.h"


static std::string		MINE_YIELD;


/**
 * 
 */
MineOperationsWindow::MineOperationsWindow(Font& font) : mBold("fonts/opensans-bold.ttf", 10), mUiIcon("ui/interface/mine.png")
{
	Control::font(font);
	text(constants::WINDOW_MINE_OPERATIONS);
	init();
}


/**
 * D'tor
 */
MineOperationsWindow::~MineOperationsWindow()
{}


/**
 * 
 */
void MineOperationsWindow::init()
{
	size(450, 275);

	// Set up GUI Layout
	addControl("btnOkay", &btnOkay, 233, 138);
	btnOkay.font(font());
	btnOkay.text("Okay");
	btnOkay.size(40, 20);
	btnOkay.click().connect(this, &MineOperationsWindow::btnOkayClicked);

	addControl("btnCancel", &btnCancel, 276, 138);
	btnCancel.font(font());
	btnCancel.text("Cancel");
	btnCancel.size(40, 20);
	btnCancel.click().connect(this, &MineOperationsWindow::btnCancelClicked);
}


/**
 * 
 */
void MineOperationsWindow::hide()
{
	Control::hide();
	mFacility = nullptr;
}


/**
 * 
 */
void MineOperationsWindow::mineFacility(MineFacility* _mf)
{
	mFacility = _mf;
	if (!mFacility) { return; }

	//MINE_YIELD = mFacility->mine()->
}


/**
 * 
 */
void MineOperationsWindow::btnOkayClicked()
{
	hide();
}


/**
 * 
 */
void MineOperationsWindow::btnCancelClicked()
{
	hide();
}


/**
 * 
 */
void MineOperationsWindow::update()
{
	if (!visible()) { return; }

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawImage(mUiIcon, rect().x() + 10, rect().y() + 30);

	r.drawText(mBold, "Mine Yield:", rect().x() + 148, rect().y() + 30, 255, 255, 255);
}
