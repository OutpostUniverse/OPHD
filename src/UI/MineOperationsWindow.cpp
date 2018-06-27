// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MineOperationsWindow.h"

#include "../Constants.h"


static std::string		MINE_YIELD;
static std::string		MINE_DEPTH;

static const int		MINE_YIELD_POSITION = 148;
static int				MINE_YIELD_DESCRIPTION_POSITION = 0;

static const int		MINE_DEPTH_POSITION = 300;
static int				MINE_DEPTH_VALUE_POSITION;


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
	size(450, 250);

	// Set up GUI Layout
	addControl("btnCancel", &btnCancel, width() - 70, 210);
	btnCancel.font(font());
	btnCancel.text("Cancel");
	btnCancel.size(60, 30);
	btnCancel.click().connect(this, &MineOperationsWindow::btnCancelClicked);

	addControl("btnOkay", &btnOkay, btnCancel.positionX() - 62, 210);
	btnOkay.font(font());
	btnOkay.text("Okay");
	btnOkay.size(60, 30);
	btnOkay.click().connect(this, &MineOperationsWindow::btnOkayClicked);

	addControl("btnExtendShaft", &btnExtendShaft, btnOkay.positionX() - 102, 210);
	btnExtendShaft.font(font());
	btnExtendShaft.text("Dig New Level");
	btnExtendShaft.size(100, 30);
	btnExtendShaft.click().connect(this, &MineOperationsWindow::btnExtendShaftClicked);
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

	MINE_YIELD = MINE_YIELD_TRANSLATION[mFacility->mine()->productionRate()];
	MINE_DEPTH = std::to_string(mFacility->mine()->depth());

	MINE_YIELD_DESCRIPTION_POSITION = MINE_YIELD_POSITION + mBold.width("Mine Yield") + 5;
	MINE_DEPTH_VALUE_POSITION = MINE_DEPTH_POSITION + mBold.width("Depth") + 5;
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
void MineOperationsWindow::btnExtendShaftClicked()
{

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

	r.drawText(mBold, "Mine Yield:", rect().x() + MINE_YIELD_POSITION, rect().y() + 30, 255, 255, 255);
	r.drawText(font(), MINE_YIELD, rect().x() + MINE_YIELD_DESCRIPTION_POSITION, rect().y() + 30, 255, 255, 255);

	r.drawText(mBold, "Depth:", rect().x() + MINE_DEPTH_POSITION, rect().y() + 30, 255, 255, 255);
	r.drawText(font(), MINE_DEPTH, rect().x() + MINE_DEPTH_VALUE_POSITION, rect().y() + 30, 255, 255, 255);
}
