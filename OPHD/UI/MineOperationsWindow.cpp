// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MineOperationsWindow.h"

#include "TextRender.h"

#include "../Constants.h"
#include "../FontManager.h"

using namespace NAS2D;

static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;


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

	auto& renderer = Utility<Renderer>::get();

	const auto origin = mRect.startPoint().to<int>();
	renderer.drawImage(mUiIcon, origin + NAS2D::Vector{10, 30});

	const auto MINE_YIELD = MINE_YIELD_TRANSLATION[mFacility->mine()->productionRate()];
	drawLabelAndValue(origin + NAS2D::Vector{MINE_YIELD_POSITION, 30}, "Mine Yield: ", MINE_YIELD);

	const std::string STATUS_STRING =
		mFacility->extending() ? "Digging New Level" :
		mFacility->mine()->exhausted() ? "Exhausted" :
		structureStateDescription(mFacility->state());
	drawLabelAndValue(origin + NAS2D::Vector{MINE_YIELD_POSITION, 45}, "Status: ", STATUS_STRING);

	if (mFacility && mFacility->extending())
	{
		const auto EXTENTION_TIME_REMAINING = std::to_string(mFacility->digTimeRemaining());
		drawLabelAndValue(origin + NAS2D::Vector{MINE_YIELD_POSITION, 60}, "Turns Remaining: ", EXTENTION_TIME_REMAINING);
	}

	const auto MINE_DEPTH = std::to_string(mFacility->mine()->depth());
	drawLabelAndValue(origin + NAS2D::Vector{MINE_DEPTH_POSITION, 30}, "Depth: ", MINE_DEPTH);

	// REMAINING ORE PANEL
	renderer.drawText(*FONT_BOLD, "Remaining Resources", {mRect.x() + 10, mRect.y() + 164}, NAS2D::Color::White);

	renderer.drawImageRect({mRect.x() + 10, mRect.y() + 180, mRect.width() - 20, 40}, mPanel);

	renderer.drawLine({mRect.x() + 98, mRect.y() + 180}, {mRect.x() + 98, mRect.y() + 219}, NAS2D::Color{22, 22, 22});
	renderer.drawLine({mRect.x() + 187, mRect.y() + 180}, {mRect.x() + 187, mRect.y() + 219}, NAS2D::Color{22, 22, 22});
	renderer.drawLine({mRect.x() + 275, mRect.y() + 180}, {mRect.x() + 275, mRect.y() + 219}, NAS2D::Color{22, 22, 22});
	
	renderer.drawLine({mRect.x() + 11, mRect.y() + 200}, {mRect.x() + mRect.width() - 11, mRect.y() + 200}, NAS2D::Color{22, 22, 22});

	const auto COMMON_METALS_COUNT = std::to_string(mFacility->mine()->commonMetalsAvailable());
	const auto COMMON_MINERALS_COUNT = std::to_string(mFacility->mine()->commonMineralsAvailable());
	const auto RARE_METALS_COUNT = std::to_string(mFacility->mine()->rareMetalsAvailable());
	const auto RARE_MINERALS_COUNT = std::to_string(mFacility->mine()->rareMineralsAvailable());

	const int COMMON_METALS_POS = 46;
	const int COMMON_MINERALS_POS = 135;
	const int RARE_METALS_POS = 224;
	const int RARE_MINERALS_POS = 312;

	const auto COMMON_METALS_ORE_POSITION = COMMON_METALS_POS - (FONT->width(COMMON_METALS_COUNT) / 2) + 8;
	const auto COMMON_MINERALS_ORE_POSITION = COMMON_MINERALS_POS - (FONT->width(COMMON_MINERALS_COUNT) / 2) + 8;
	const auto RARE_METALS_ORE_POSITION = RARE_METALS_POS - (FONT->width(RARE_METALS_COUNT) / 2) + 8;
	const auto RARE_MINERALS_ORE_POSITION = RARE_MINERALS_POS - (FONT->width(RARE_MINERALS_COUNT) / 2) + 8;

	const auto CommonMetalIconRect = NAS2D::Rectangle{64, 0, 16, 16};
	const auto CommonMineralIconRect = NAS2D::Rectangle{96, 0, 16, 16};
	const auto RareMetalIconRect = NAS2D::Rectangle{80, 0, 16, 16};
	const auto RareMineralIconRect = NAS2D::Rectangle{112, 0, 16, 16};

	renderer.drawSubImage(mIcons, origin + NAS2D::Vector{COMMON_METALS_POS, 183}, CommonMetalIconRect);
	renderer.drawSubImage(mIcons, origin + NAS2D::Vector{COMMON_MINERALS_POS, 183}, CommonMineralIconRect);
	renderer.drawSubImage(mIcons, origin + NAS2D::Vector{RARE_METALS_POS, 183}, RareMetalIconRect);
	renderer.drawSubImage(mIcons, origin + NAS2D::Vector{RARE_MINERALS_POS, 183}, RareMineralIconRect);

	renderer.drawText(*FONT, COMMON_METALS_COUNT, origin + NAS2D::Vector{COMMON_METALS_ORE_POSITION, 202}, NAS2D::Color::White);
	renderer.drawText(*FONT, COMMON_MINERALS_COUNT, origin + NAS2D::Vector{COMMON_MINERALS_ORE_POSITION, 202}, NAS2D::Color::White);
	renderer.drawText(*FONT, RARE_METALS_COUNT, origin + NAS2D::Vector{RARE_METALS_ORE_POSITION, 202}, NAS2D::Color::White);
	renderer.drawText(*FONT, RARE_MINERALS_COUNT, origin + NAS2D::Vector{RARE_MINERALS_ORE_POSITION, 202}, NAS2D::Color::White);
}
