// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MineOperationsWindow.h"

#include "TextRender.h"

#include "../Constants.h"
#include "../FontManager.h"

using namespace NAS2D;

static const Font* FONT = nullptr;
static const Font* FONT_BOLD = nullptr;


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

	add(&btnOkay, mRect.width - 70, 230);
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

	FONT = &Utility<FontManager>::get().load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	FONT_BOLD = &Utility<FontManager>::get().load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
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

	auto& renderer = Utility<Renderer>::get();

	const auto origin = mRect.startPoint();
	renderer.drawImage(mUiIcon, origin + NAS2D::Vector{10, 30});

	const auto mineYield = MINE_YIELD_TRANSLATION[mFacility->mine()->productionRate()];
	drawLabelAndValue(origin + NAS2D::Vector{148, 30}, "Mine Yield: ", mineYield);

	const std::string statusString =
		mFacility->extending() ? "Digging New Level" :
		mFacility->mine()->exhausted() ? "Exhausted" :
		structureStateDescription(mFacility->state());
	drawLabelAndValue(origin + NAS2D::Vector{148, 45}, "Status: ", statusString);

	if (mFacility && mFacility->extending())
	{
		const auto extensionTimeRemaining = std::to_string(mFacility->digTimeRemaining());
		drawLabelAndValue(origin + NAS2D::Vector{148, 60}, "Turns Remaining: ", extensionTimeRemaining);
	}

	const auto mineDepth = std::to_string(mFacility->mine()->depth());
	drawLabelAndValue(origin + NAS2D::Vector{300, 30}, "Depth: ", mineDepth);

	// REMAINING ORE PANEL
	const auto width = mRect.width;
	renderer.drawText(*FONT_BOLD, "Remaining Resources", origin + NAS2D::Vector{10, 164}, NAS2D::Color::White);

	renderer.drawImageRect(NAS2D::Rectangle<int>::Create(origin + NAS2D::Vector{10, 180}, NAS2D::Vector{width - 20, 40}), mPanel);

	renderer.drawLine(origin + NAS2D::Vector{98, 180}, origin + NAS2D::Vector{98, 219}, NAS2D::Color{22, 22, 22});
	renderer.drawLine(origin + NAS2D::Vector{187, 180}, origin + NAS2D::Vector{187, 219}, NAS2D::Color{22, 22, 22});
	renderer.drawLine(origin + NAS2D::Vector{275, 180}, origin + NAS2D::Vector{275, 219}, NAS2D::Color{22, 22, 22});
	
	renderer.drawLine(origin + NAS2D::Vector{11, 200}, origin + NAS2D::Vector{width - 11, 200}, NAS2D::Color{22, 22, 22});

	const auto CommonMetalIconRect = NAS2D::Rectangle{64, 0, 16, 16};
	const auto CommonMineralIconRect = NAS2D::Rectangle{96, 0, 16, 16};
	const auto RareMetalIconRect = NAS2D::Rectangle{80, 0, 16, 16};
	const auto RareMineralIconRect = NAS2D::Rectangle{112, 0, 16, 16};

	const std::array resources{
		std::tuple{46,  CommonMetalIconRect,   mFacility->mine()->commonMetalsAvailable()},
		std::tuple{135, CommonMineralIconRect, mFacility->mine()->commonMineralsAvailable()},
		std::tuple{224, RareMetalIconRect,     mFacility->mine()->rareMetalsAvailable()},
		std::tuple{313, RareMineralIconRect,   mFacility->mine()->rareMineralsAvailable()}
	};

	for (const auto& [offsetX, iconRect, resourceCount] : resources) {
		const auto resourceCountString = std::to_string(resourceCount);
		const auto textOffsetX = offsetX - (FONT->width(resourceCountString) / 2) + 8;
		renderer.drawSubImage(mIcons, origin + NAS2D::Vector{offsetX, 183}, iconRect);
		renderer.drawText(*FONT, resourceCountString, origin + NAS2D::Vector{textOffsetX, 202}, NAS2D::Color::White);
	}
}
