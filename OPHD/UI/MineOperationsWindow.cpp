#include "MineOperationsWindow.h"

#include "StringTable.h"
#include "TextRender.h"

#include "../Cache.h"
#include "../Common.h"
#include "../Constants.h"
#include "../StructureManager.h"
#include "../Things/Structures/MineFacility.h"
#include "../Things/Structures/Warehouse.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


MineOperationsWindow::MineOperationsWindow() :
	Window{constants::WindowMineOperations},
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	mFontBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryNormal)},
	mUiIcon{imageCache.load("ui/interface/mine.png")},
	mIcons{imageCache.load("ui/icons.png")},
	mPanel{
		imageCache.load("ui/skin/textbox_top_left.png"),
		imageCache.load("ui/skin/textbox_top_middle.png"),
		imageCache.load("ui/skin/textbox_top_right.png"),
		imageCache.load("ui/skin/textbox_middle_left.png"),
		imageCache.load("ui/skin/textbox_middle_middle.png"),
		imageCache.load("ui/skin/textbox_middle_right.png"),
		imageCache.load("ui/skin/textbox_bottom_left.png"),
		imageCache.load("ui/skin/textbox_bottom_middle.png"),
		imageCache.load("ui/skin/textbox_bottom_right.png")
	},
	chkCommonMetals{ResourceNamesRefined[0]},
	chkCommonMinerals{ResourceNamesRefined[1]},
	chkRareMetals{ResourceNamesRefined[2]},
	chkRareMinerals{ResourceNamesRefined[3]},
	btnIdle{"Idle", {this, &MineOperationsWindow::onIdle}},
	btnExtendShaft{"Dig New Level", {this, &MineOperationsWindow::onExtendShaft}},
	btnOkay{"Close", {this, &MineOperationsWindow::onOkay}},
	btnAssignTruck{"Add Truck", {this, &MineOperationsWindow::onAssignTruck}},
	btnUnassignTruck{"Remove Truck", {this, &MineOperationsWindow::onUnassignTruck}}
{
	size({375, 270});

	// Set up GUI Layout
	add(btnIdle, {10, 230});
	btnIdle.type(Button::Type::BUTTON_TOGGLE);
	btnIdle.size({60, 30});

	add(btnExtendShaft, {72, 230});
	btnExtendShaft.size({100, 30});

	add(btnOkay, {mRect.width - 70, 230});
	btnOkay.size({60, 30});

	add(btnAssignTruck, {mRect.width - 85, 115});
	btnAssignTruck.size({80, 20});

	add(btnUnassignTruck, {mRect.width - 170, 115});
	btnUnassignTruck.size({80, 20});

	// ORE TOGGLE BUTTONS
	add(chkCommonMetals, {148, 140});
	chkCommonMetals.click().connect(this, &MineOperationsWindow::onCheckBoxCommonMetalsChange);

	add(chkCommonMinerals, {259, 140});
	chkCommonMinerals.click().connect(this, &MineOperationsWindow::onCheckBoxCommonMineralsChange);

	add(chkRareMetals, {148, 160});
	chkRareMetals.click().connect(this, &MineOperationsWindow::onCheckBoxRareMetalsChange);

	add(chkRareMinerals, {259, 160});
	chkRareMinerals.click().connect(this, &MineOperationsWindow::onCheckBoxRareMineralsChange);
}


void MineOperationsWindow::hide()
{
	Control::hide();
	mFacility = nullptr;
}


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

	updateTruckAvailability();
}


void MineOperationsWindow::onOkay()
{
	hide();
}


void MineOperationsWindow::onExtendShaft()
{
	mFacility->extend();
	btnExtendShaft.enabled(false);
}


void MineOperationsWindow::onIdle()
{
	mFacility->forceIdle(btnIdle.toggled());
}



void MineOperationsWindow::onAssignTruck()
{
	if (mFacility->assignedTrucks() == mFacility->maxTruckCount()) { return; }

	if (pullTruckFromInventory())
	{
		mFacility->addTruck();
		updateTruckAvailability();
	}
}


void MineOperationsWindow::onUnassignTruck()
{
	if (mFacility->assignedTrucks() == 1) { return; }

	if (pushTruckIntoInventory())
	{
		mFacility->removeTruck();
		updateTruckAvailability();
	}
}


void MineOperationsWindow::onCheckBoxCommonMetalsChange()
{
	mFacility->mine()->miningCommonMetals(chkCommonMetals.checked());
}


void MineOperationsWindow::onCheckBoxCommonMineralsChange()
{
	mFacility->mine()->miningCommonMinerals(chkCommonMinerals.checked());
}


void MineOperationsWindow::onCheckBoxRareMetalsChange()
{
	mFacility->mine()->miningRareMetals(chkRareMetals.checked());
}


void MineOperationsWindow::onCheckBoxRareMineralsChange()
{
	mFacility->mine()->miningRareMinerals(chkRareMinerals.checked());
}


void MineOperationsWindow::updateTruckAvailability()
{
	mAvailableTrucks = getTruckAvailability();
}


void MineOperationsWindow::update()
{
	if (!visible()) { return; }

	Window::update();

	auto& renderer = Utility<Renderer>::get();

	const auto origin = mRect.startPoint();
	renderer.drawImage(mUiIcon, origin + NAS2D::Vector{10, 30});

	const auto& mineYield = MINE_YIELD_TRANSLATION.at(mFacility->mine()->productionRate());
	drawLabelAndValue(origin + NAS2D::Vector{148, 30}, "Mine Yield: ", mineYield);

	const std::string statusString =
		mFacility->extending() ? "Digging New Level" :
		mFacility->mine()->exhausted() ? "Exhausted" :
		mFacility->stateDescription();

	drawLabelAndValue(origin + NAS2D::Vector{148, 45}, "Status: ", statusString);

	if (mFacility && mFacility->extending())
	{
		const auto extensionTimeRemaining = std::to_string(mFacility->digTimeRemaining());
		drawLabelAndValue(origin + NAS2D::Vector{148, 60}, "Turns Remaining: ", extensionTimeRemaining);
	}

	const auto mineDepth = std::to_string(mFacility->mine()->depth());
	drawLabelAndValue(origin + NAS2D::Vector{300, 30}, "Depth: ", mineDepth);

	// TRUCK ASSIGNMENT
	renderer.drawText(mFontBold, "Trucks", origin + NAS2D::Vector{148, 80}, NAS2D::Color::White);
	drawLabelAndValue(origin + NAS2D::Vector{148, 95}, "Assigned: ", std::to_string(mFacility->assignedTrucks()));
	drawLabelAndValue(origin + NAS2D::Vector{260, 95}, "Available: ", std::to_string(mAvailableTrucks));

	// REMAINING ORE PANEL
	const auto width = mRect.width;
	renderer.drawText(mFontBold, "Remaining Resources", origin + NAS2D::Vector{10, 164}, NAS2D::Color::White);

	mPanel.draw(renderer, NAS2D::Rectangle<int>::Create(origin + NAS2D::Vector{10, 180}, NAS2D::Vector{width - 20, 40}));

	renderer.drawLine(origin + NAS2D::Vector{98, 180}, origin + NAS2D::Vector{98, 219}, NAS2D::Color{22, 22, 22});
	renderer.drawLine(origin + NAS2D::Vector{187, 180}, origin + NAS2D::Vector{187, 219}, NAS2D::Color{22, 22, 22});
	renderer.drawLine(origin + NAS2D::Vector{275, 180}, origin + NAS2D::Vector{275, 219}, NAS2D::Color{22, 22, 22});

	renderer.drawLine(origin + NAS2D::Vector{11, 200}, origin + NAS2D::Vector{width - 11, 200}, NAS2D::Color{22, 22, 22});

	const auto availableResources = mFacility->mine()->availableResources();
	const std::array resources
	{
		std::tuple{46,  ResourceImageRectsOre[0], availableResources.resources[0]},
		std::tuple{135, ResourceImageRectsOre[1], availableResources.resources[1]},
		std::tuple{224, ResourceImageRectsOre[2], availableResources.resources[2]},
		std::tuple{313, ResourceImageRectsOre[3], availableResources.resources[3]}
	};

	for (const auto& [offsetX, iconRect, resourceCount] : resources)
	{
		const auto resourceCountString = std::to_string(resourceCount);
		const auto textOffsetX = offsetX - (mFont.width(resourceCountString) / 2) + 8;
		renderer.drawSubImage(mIcons, origin + NAS2D::Vector{offsetX, 183}, iconRect);
		renderer.drawText(mFont, resourceCountString, origin + NAS2D::Vector{textOffsetX, 202}, NAS2D::Color::White);
	}
}
