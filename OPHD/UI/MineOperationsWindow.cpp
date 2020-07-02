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

static int COMMON_METALS_ORE_POSITION;
static int COMMON_MINERALS_ORE_POSITION;
static int RARE_METALS_ORE_POSITION;
static int RARE_MINERALS_ORE_POSITION;

static std::string MINE_YIELD;

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

	COMMON_METALS_COUNT = std::to_string(mFacility->mine()->commonMetalsAvailable());
	COMMON_MINERALS_COUNT = std::to_string(mFacility->mine()->commonMineralsAvailable());
	RARE_METALS_COUNT = std::to_string(mFacility->mine()->rareMetalsAvailable());
	RARE_MINERALS_COUNT = std::to_string(mFacility->mine()->rareMineralsAvailable());

	COMMON_METALS_ORE_POSITION = COMMON_METALS_POS - (FONT->width(COMMON_METALS_COUNT) / 2) + 8;
	COMMON_MINERALS_ORE_POSITION = COMMON_MINERALS_POS - (FONT->width(COMMON_MINERALS_COUNT) / 2) + 8;
	RARE_METALS_ORE_POSITION = RARE_METALS_POS - (FONT->width(RARE_METALS_COUNT) / 2) + 8;
	RARE_MINERALS_ORE_POSITION = RARE_MINERALS_POS - (FONT->width(RARE_MINERALS_COUNT) / 2) + 8;
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

	const int MINE_YIELD_POSITION = 148;
	const int MINE_DEPTH_POSITION = 300;

	const int MINE_YIELD_DESCRIPTION_POSITION = MINE_YIELD_POSITION + FONT_BOLD->width("Mine Yield:") + 10;
	const int MINE_DEPTH_VALUE_POSITION = MINE_DEPTH_POSITION + FONT_BOLD->width("Depth:") + 10;

	const int MINE_STATUS_POSITION = MINE_YIELD_POSITION + FONT_BOLD->width("Status:") + 10;
	const int EXTENSION_TURNS_REMAINING_POSITION = MINE_YIELD_POSITION + FONT_BOLD->width("Turns Remaining:") + 10;

	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(mUiIcon, mRect.x() + 10, mRect.y() + 30);

	renderer.drawText(*FONT_BOLD, "Mine Yield:", {mRect.x() + MINE_YIELD_POSITION, mRect.y() + 30}, NAS2D::Color::White);
	renderer.drawText(*FONT, MINE_YIELD, {mRect.x() + MINE_YIELD_DESCRIPTION_POSITION, mRect.y() + 30}, NAS2D::Color::White);

	renderer.drawText(*FONT_BOLD, "Status:", {mRect.x() + MINE_YIELD_POSITION, mRect.y() + 45}, NAS2D::Color::White);

	const std::string STATUS_STRING =
		mFacility->extending() ? "Digging New Level" :
		mFacility->mine()->exhausted() ? "Exhausted" :
		structureStateDescription(mFacility->state());
	renderer.drawText(*FONT, STATUS_STRING, {mRect.x() + MINE_STATUS_POSITION, mRect.y() + 45}, NAS2D::Color::White);

	if (mFacility && mFacility->extending())
	{
		renderer.drawText(*FONT_BOLD, "Turns Remaining:", {mRect.x() + MINE_YIELD_POSITION, mRect.y() + 60}, NAS2D::Color::White);
		const auto EXTENTION_TIME_REMAINING = std::to_string(mFacility->digTimeRemaining());
		renderer.drawText(*FONT, EXTENTION_TIME_REMAINING, {mRect.x() + EXTENSION_TURNS_REMAINING_POSITION, mRect.y() + 60}, NAS2D::Color::White);
	}

	const auto MINE_DEPTH = std::to_string(mFacility->mine()->depth());
	renderer.drawText(*FONT_BOLD, "Depth:", {mRect.x() + MINE_DEPTH_POSITION, mRect.y() + 30}, NAS2D::Color::White);
	renderer.drawText(*FONT, MINE_DEPTH, {mRect.x() + MINE_DEPTH_VALUE_POSITION, mRect.y() + 30}, NAS2D::Color::White);

	// REMAINING ORE PANEL
	renderer.drawText(*FONT_BOLD, "Remaining Resources", {mRect.x() + 10, mRect.y() + 164}, NAS2D::Color::White);

	renderer.drawImageRect({mRect.x() + 10, mRect.y() + 180, mRect.width() - 20, 40}, mPanel);

	renderer.drawLine({mRect.x() + 98, mRect.y() + 180}, {mRect.x() + 98, mRect.y() + 219}, NAS2D::Color{22, 22, 22});
	renderer.drawLine({mRect.x() + 187, mRect.y() + 180}, {mRect.x() + 187, mRect.y() + 219}, NAS2D::Color{22, 22, 22});
	renderer.drawLine({mRect.x() + 275, mRect.y() + 180}, {mRect.x() + 275, mRect.y() + 219}, NAS2D::Color{22, 22, 22});
	
	renderer.drawLine({mRect.x() + 11, mRect.y() + 200}, {mRect.x() + mRect.width() - 11, mRect.y() + 200}, NAS2D::Color{22, 22, 22});

	renderer.drawSubImage(mIcons, {mRect.x() + COMMON_METALS_POS, mRect.y() + 183}, {64, 0, 16, 16});
	renderer.drawSubImage(mIcons, {mRect.x() + COMMON_MINERALS_POS, mRect.y() + 183}, {96, 0, 16, 16});
	renderer.drawSubImage(mIcons, {mRect.x() + RARE_METALS_POS, mRect.y() + 183}, {80, 0, 16, 16});
	renderer.drawSubImage(mIcons, {mRect.x() + RARE_MINERALS_POS, mRect.y() + 183}, {112, 0, 16, 16});

	renderer.drawText(*FONT, COMMON_METALS_COUNT, {mRect.x() + COMMON_METALS_ORE_POSITION, mRect.y() + 202}, NAS2D::Color::White);
	renderer.drawText(*FONT, COMMON_MINERALS_COUNT, {mRect.x() + COMMON_MINERALS_ORE_POSITION, mRect.y() + 202}, NAS2D::Color::White);
	renderer.drawText(*FONT, RARE_METALS_COUNT, {mRect.x() + RARE_METALS_ORE_POSITION, mRect.y() + 202}, NAS2D::Color::White);
	renderer.drawText(*FONT, RARE_MINERALS_COUNT, {mRect.x() + RARE_MINERALS_ORE_POSITION, mRect.y() + 202}, NAS2D::Color::White);
}
