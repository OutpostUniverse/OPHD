// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MineOperationsWindow.h"

#include "../Constants.h"
#include "../FontManager.h"

using namespace NAS2D;

/**
 * These static variables are here to avoid unnecessary computations
 * during frame updates to improve overall performance (basically, to avoid
 * expensive string operations).
 */
static std::string COMMON_METALS_COUNT;
static std::string COMMON_MINERALS_COUNT;
static std::string RARE_METALS_COUNT;
static std::string RARE_MINERALS_COUNT;

static std::string STATUS_STRING;
static std::string EXTENTION_TIME_REMAINING;

static int COMMON_METALS_ORE_POSITION;
static int COMMON_MINERALS_ORE_POSITION;
static int RARE_METALS_ORE_POSITION;
static int RARE_MINERALS_ORE_POSITION;

static std::string MINE_YIELD;
static std::string MINE_DEPTH;

static const int MINE_YIELD_POSITION = 148;
static int MINE_YIELD_DESCRIPTION_POSITION = 0;

static int MINE_STATUS_POSITION;
static int EXTENSION_TURNS_REMAINING_POSITION;

static const int MINE_DEPTH_POSITION = 300;
static int MINE_DEPTH_VALUE_POSITION;

static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;


/** 
 * Positional constants used in several places.
 */
const int COMMON_METALS_POS = 46;
const int COMMON_MINERALS_POS = 135;
const int RARE_METALS_POS = 224;
const int RARE_MINERALS_POS = 312;


/**
 * 
 */
MineOperationsWindow::MineOperationsWindow() :
	mUiIcon{"ui/interface/mine.png"},
	mIcons{"ui/icons.png"},
	chkCommonMetals{"Common Metals"},
	chkCommonMinerals{"Common Minerals"},
	chkRareMetals{"Rare Metals"},
	chkRareMinerals{"Rare Minerals"},
	btnIdle{"Idle"},
	btnExtendShaft{"Dig New Level"},
	btnOkay{"Close"}
{
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
	size({375, 270});

	// Set up GUI Layout
	add(&btnIdle, 10, 230);
	btnIdle.type(Button::Type::BUTTON_TOGGLE);
	btnIdle.size({60, 30});
	btnIdle.click().connect(this, &MineOperationsWindow::btnIdleClicked);

	add(&btnExtendShaft, 72, 230);
	btnExtendShaft.size({100, 30});
	btnExtendShaft.click().connect(this, &MineOperationsWindow::btnExtendShaftClicked);

	add(&btnOkay, width() - 70, 230);
	btnOkay.size({60, 30});
	btnOkay.click().connect(this, &MineOperationsWindow::btnOkayClicked);

	// ORE TOGGLE BUTTONS
	add(&chkCommonMetals, 148, 93);
	chkCommonMetals.click().connect(this, &MineOperationsWindow::chkCommonMetalsClicked);

	add(&chkCommonMinerals, 259, 93);
	chkCommonMinerals.click().connect(this, &MineOperationsWindow::chkCommonMineralsClicked);

	add(&chkRareMetals, 148, 128);
	chkRareMetals.click().connect(this, &MineOperationsWindow::chkRareMetalsClicked);

	add(&chkRareMinerals, 259, 128);
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

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
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
void MineOperationsWindow::mineFacility(MineFacility* facility)
{
	mFacility = facility;
	if (!mFacility) { return; }

	MINE_YIELD = MINE_YIELD_TRANSLATION[mFacility->mine()->productionRate()];
	MINE_DEPTH = std::to_string(mFacility->mine()->depth());

	MINE_YIELD_DESCRIPTION_POSITION = MINE_YIELD_POSITION + FONT_BOLD->width("Mine Yield:") + 10;
	MINE_DEPTH_VALUE_POSITION = MINE_DEPTH_POSITION + FONT_BOLD->width("Depth:") + 10;

	MINE_STATUS_POSITION = MINE_YIELD_POSITION + FONT_BOLD->width("Status:") + 10;
	EXTENSION_TURNS_REMAINING_POSITION = MINE_YIELD_POSITION + FONT_BOLD->width("Turns Remaining:") + 10;


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

	COMMON_METALS_ORE_POSITION = COMMON_METALS_POS - (FONT->width(COMMON_METALS_COUNT) / 2) + 8;
	COMMON_MINERALS_ORE_POSITION = COMMON_MINERALS_POS - (FONT->width(COMMON_MINERALS_COUNT) / 2) + 8;
	RARE_METALS_ORE_POSITION = RARE_METALS_POS - (FONT->width(RARE_METALS_COUNT) / 2) + 8;
	RARE_MINERALS_ORE_POSITION = RARE_MINERALS_POS - (FONT->width(RARE_MINERALS_COUNT) / 2) + 8;

	if (mFacility->extending()) { STATUS_STRING = "Digging New Level"; }
	else { STATUS_STRING = structureStateDescription(mFacility->state()); }

	EXTENTION_TIME_REMAINING = std::to_string(mFacility->digTimeRemaining());
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
	updateCounts();
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

	r.drawText(*FONT_BOLD, "Mine Yield:", rect().x() + MINE_YIELD_POSITION, rect().y() + 30, 255, 255, 255);
	r.drawText(*FONT, MINE_YIELD, rect().x() + MINE_YIELD_DESCRIPTION_POSITION, rect().y() + 30, 255, 255, 255);

	r.drawText(*FONT_BOLD, "Status:", rect().x() + MINE_YIELD_POSITION, rect().y() + 45, 255, 255, 255);

	if (mFacility->extending()) { STATUS_STRING = "Digging New Level"; }
	else if (mFacility->mine()->exhausted()) { STATUS_STRING = "Exhausted"; }
	else { STATUS_STRING = structureStateDescription(mFacility->state()); }
	
	r.drawText(*FONT, STATUS_STRING, rect().x() + MINE_STATUS_POSITION, rect().y() + 45, 255, 255, 255);

	if (mFacility && mFacility->extending())
	{
		r.drawText(*FONT_BOLD, "Turns Remaining:", rect().x() + MINE_YIELD_POSITION, rect().y() + 60, 255, 255, 255);
		r.drawText(*FONT, EXTENTION_TIME_REMAINING, rect().x() + EXTENSION_TURNS_REMAINING_POSITION, rect().y() + 60, 255, 255, 255);
	}

	r.drawText(*FONT_BOLD, "Depth:", rect().x() + MINE_DEPTH_POSITION, rect().y() + 30, 255, 255, 255);
	r.drawText(*FONT, MINE_DEPTH, rect().x() + MINE_DEPTH_VALUE_POSITION, rect().y() + 30, 255, 255, 255);

	// REMAINING ORE PANEL
	r.drawText(*FONT_BOLD, "Remaining Resources", rect().x() + 10, rect().y() + 164, 255, 255, 255);

	r.drawImageRect(rect().x() + 10, rect().y() + 180, rect().width() - 20, 40, mPanel);

	r.drawLine(rect().x() + 98, rect().y() + 180, rect().x() + 98, rect().y() + 219, 22, 22, 22);
	r.drawLine(rect().x() + 187, rect().y() + 180, rect().x() + 187, rect().y() + 219, 22, 22, 22);
	r.drawLine(rect().x() + 275, rect().y() + 180, rect().x() + 275, rect().y() + 219, 22, 22, 22);
	
	r.drawLine(rect().x() + 11, rect().y() + 200, rect().x() + rect().width() - 11, rect().y() + 200, 22, 22, 22);

	r.drawSubImage(mIcons, rect().x() + COMMON_METALS_POS, rect().y() + 183, 64, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + COMMON_MINERALS_POS, rect().y() + 183, 96, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + RARE_METALS_POS, rect().y() + 183, 80, 0, 16, 16);
	r.drawSubImage(mIcons, rect().x() + RARE_MINERALS_POS, rect().y() + 183, 112, 0, 16, 16);

	r.drawText(*FONT, COMMON_METALS_COUNT, rect().x() + COMMON_METALS_ORE_POSITION, rect().y() + 202, 255, 255, 255);
	r.drawText(*FONT, COMMON_MINERALS_COUNT, rect().x() + COMMON_MINERALS_ORE_POSITION, rect().y() + 202, 255, 255, 255);
	r.drawText(*FONT, RARE_METALS_COUNT, rect().x() + RARE_METALS_ORE_POSITION, rect().y() + 202, 255, 255, 255);
	r.drawText(*FONT, RARE_MINERALS_COUNT, rect().x() + RARE_MINERALS_ORE_POSITION, rect().y() + 202, 255, 255, 255);
}
