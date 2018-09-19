// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MineOperationsWindow.h"

#include "../Constants.h"

/**
 * These static variables are here to avoid unnecessary computations
 * during frame updates to improve overall performance (basically, to avoid
 * expensive string operations).
 */
static std::string		COMMON_METALS_COUNT;
static std::string		COMMON_MINERALS_COUNT;
static std::string		RARE_METALS_COUNT;
static std::string		RARE_MINERALS_COUNT;

static int				COMMON_METALS_ORE_POSITION;
static int				COMMON_MINERALS_ORE_POSITION;
static int				RARE_METALS_ORE_POSITION;
static int				RARE_MINERALS_ORE_POSITION;

static std::string		MINE_YIELD;
static std::string		MINE_DEPTH;

static const int		MINE_YIELD_POSITION = 148;
static int				MINE_YIELD_DESCRIPTION_POSITION = 0;

static const int		MINE_DEPTH_POSITION = 300;
static int				MINE_DEPTH_VALUE_POSITION;


/** 
 * Positional constants used in several places.
 */
const int				COMMON_METALS_POS = 46;
const int				COMMON_MINERALS_POS = 135;
const int				RARE_METALS_POS = 224;
const int				RARE_MINERALS_POS = 312;


/**
 * 
 */
MineOperationsWindow::MineOperationsWindow(Font& font) : mBold("fonts/opensans-bold.ttf", 10), mUiIcon("ui/interface/mine.png"), mIcons("ui/icons.png")
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
	size(375, 270);

	// Set up GUI Layout
	add(&btnIdle, 10, 230);
	btnIdle.type(Button::BUTTON_TOGGLE);
	btnIdle.font(font());
	btnIdle.text("Idle");
	btnIdle.size(60, 30);
	btnIdle.click().connect(this, &MineOperationsWindow::btnIdleClicked);

	add(&btnExtendShaft, 72, 230);
	btnExtendShaft.font(font());
	btnExtendShaft.text("Dig New Level");
	btnExtendShaft.size(100, 30);
	btnExtendShaft.click().connect(this, &MineOperationsWindow::btnExtendShaftClicked);

	add(&btnOkay, width() - 70, 230);
	btnOkay.font(font());
	btnOkay.text("Close");
	btnOkay.size(60, 30);
	btnOkay.click().connect(this, &MineOperationsWindow::btnOkayClicked);

	// ORE TOGGLE BUTTONS
	add(&chkCommonMetals, 148, 93);
	chkCommonMetals.text("Common Metals");
	chkCommonMetals.click().connect(this, &MineOperationsWindow::chkCommonMetalsClicked);

	add(&chkCommonMinerals, 259, 93);
	chkCommonMinerals.text("Common Minerals");
	chkCommonMinerals.click().connect(this, &MineOperationsWindow::chkCommonMineralsClicked);

	add(&chkRareMetals, 148, 128);
	chkRareMetals.text("Rare Metals");
	chkRareMetals.click().connect(this, &MineOperationsWindow::chkRareMetalsClicked);

	add(&chkRareMinerals, 259, 128);
	chkRareMinerals.text("Rare Minerals");
	chkRareMinerals.click().connect(this, &MineOperationsWindow::chkRareMineralsClicked);

	mPanel.push_back(Image("ui/skin/textbox_top_left.png"));
	mPanel.push_back(Image("ui/skin/textbox_top_middle.png"));
	mPanel.push_back(Image("ui/skin/textbox_top_right.png"));
	mPanel.push_back(Image("ui/skin/textbox_middle_left.png"));
	mPanel.push_back(Image("ui/skin/textbox_middle_middle.png"));
	mPanel.push_back(Image("ui/skin/textbox_middle_right.png"));
	mPanel.push_back(Image("ui/skin/textbox_bottom_left.png"));
	mPanel.push_back(Image("ui/skin/textbox_bottom_middle.png"));
	mPanel.push_back(Image("ui/skin/textbox_bottom_right.png"));
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

	MINE_YIELD_DESCRIPTION_POSITION = MINE_YIELD_POSITION + mBold.width("Mine Yield:") + 10;
	MINE_DEPTH_VALUE_POSITION = MINE_DEPTH_POSITION + mBold.width("Depth:") + 10;

	chkCommonMetals.checked(mFacility->mine()->miningCommonMetals());
	chkCommonMinerals.checked(mFacility->mine()->miningCommonMinerals());
	chkRareMetals.checked(mFacility->mine()->miningRareMetals());
	chkRareMinerals.checked(mFacility->mine()->miningRareMinerals());

	btnIdle.toggle(mFacility->forceIdle());
	btnExtendShaft.enabled(mFacility->canExtend());

	updateCounts();
}


/**
 * Called at the end of each turn after mine processing
 * to update the static strings for ore remaining.
 */
void MineOperationsWindow::updateCounts()
{
	if (!visible() && !mFacility) { return; }

	MINE_DEPTH = std::to_string(mFacility->mine()->depth());
	
	COMMON_METALS_COUNT = std::to_string(mFacility->mine()->commonMetalsAvailable());
	COMMON_MINERALS_COUNT = std::to_string(mFacility->mine()->commonMineralsAvailable());
	RARE_METALS_COUNT = std::to_string(mFacility->mine()->rareMetalsAvailable());
	RARE_MINERALS_COUNT = std::to_string(mFacility->mine()->rareMineralsAvailable());

	COMMON_METALS_ORE_POSITION = COMMON_METALS_POS - (font().width(COMMON_METALS_COUNT) / 2) + 8;
	COMMON_MINERALS_ORE_POSITION = COMMON_MINERALS_POS - (font().width(COMMON_MINERALS_COUNT) / 2) + 8;
	RARE_METALS_ORE_POSITION = RARE_METALS_POS - (font().width(RARE_METALS_COUNT) / 2) + 8;
	RARE_MINERALS_ORE_POSITION = RARE_MINERALS_POS - (font().width(RARE_MINERALS_COUNT) / 2) + 8;
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
void MineOperationsWindow::btnExtendShaftClicked()
{
	mFacility->extend();
	btnExtendShaft.enabled(false);
}


/**
 * 
 */
void MineOperationsWindow::btnIdleClicked()
{
	mFacility->forceIdle(btnIdle.toggled());
}


/**
 * 
 */
void MineOperationsWindow::chkCommonMetalsClicked()
{
	mFacility->mine()->miningCommonMetals(chkCommonMetals.checked());
}


/**
 * 
 */
void MineOperationsWindow::chkCommonMineralsClicked()
{
	mFacility->mine()->miningCommonMinerals(chkCommonMinerals.checked());
}


/**
 * 
 */
void MineOperationsWindow::chkRareMetalsClicked()
{
	mFacility->mine()->miningRareMetals(chkRareMetals.checked());
}


/**
 * 
 */
void MineOperationsWindow::chkRareMineralsClicked()
{
	mFacility->mine()->miningRareMinerals(chkRareMinerals.checked());
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

	// REMAINING ORE PANEL
	r.drawText(mBold, "Remaining Resources", rect().x() + 10, rect().y() + 164, 255, 255, 255);

	r.drawImageRect(rect().x() + 10, rect().y() + 180, rect().width() - 20, 40, mPanel);

	r.drawLine(rect().x() + 98, rect().y() + 180, rect().x() + 98, rect().y() + 219, 22, 22, 22);
	r.drawLine(rect().x() + 187, rect().y() + 180, rect().x() + 187, rect().y() + 219, 22, 22, 22);
	r.drawLine(rect().x() + 275, rect().y() + 180, rect().x() + 275, rect().y() + 219, 22, 22, 22);
	
	r.drawLine(rect().x() + 11, rect().y() + 200, rect().x() + rect().width() - 11, rect().y() + 200, 22, 22, 22);

	r.drawSubImage(mIcons, rect().x() + COMMON_METALS_POS, rect().y() + 183, 64, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + COMMON_MINERALS_POS, rect().y() + 183, 96, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + RARE_METALS_POS, rect().y() + 183, 80, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + RARE_MINERALS_POS, rect().y() + 183, 112, 0, 16, 16);

	r.drawText(font(), COMMON_METALS_COUNT, rect().x() + COMMON_METALS_ORE_POSITION, rect().y() + 202, 255, 255, 255);
	r.drawText(font(), COMMON_MINERALS_COUNT, rect().x() + COMMON_MINERALS_ORE_POSITION, rect().y() + 202, 255, 255, 255);
	r.drawText(font(), RARE_METALS_COUNT, rect().x() + RARE_METALS_ORE_POSITION, rect().y() + 202, 255, 255, 255);
	r.drawText(font(), RARE_MINERALS_COUNT, rect().x() + RARE_MINERALS_ORE_POSITION, rect().y() + 202, 255, 255, 255);
}
