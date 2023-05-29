#include "MineOperationsWindow.h"

#include "StringTable.h"
#include "TextRender.h"

#include "../Cache.h"
#include "../Common.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../StructureManager.h"
#include "../MapObjects/Structures/MineFacility.h"
#include "../MapObjects/Structures/Warehouse.h"

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
	chkResources{{
		{ResourceNamesRefined[0], {this, &MineOperationsWindow::onCheckBoxCommonMetalsChange}},
		{ResourceNamesRefined[1], {this, &MineOperationsWindow::onCheckBoxCommonMineralsChange}},
		{ResourceNamesRefined[2], {this, &MineOperationsWindow::onCheckBoxRareMetalsChange}},
		{ResourceNamesRefined[3], {this, &MineOperationsWindow::onCheckBoxRareMineralsChange}},
	}},
	btnIdle{"Idle", {this, &MineOperationsWindow::onIdle}},
	btnExtendShaft{"Dig New Level", {this, &MineOperationsWindow::onExtendShaft}},
	btnOkay{"Close", {this, &MineOperationsWindow::onOkay}},
	btnAssignTruck{"Add Truck", {this, &MineOperationsWindow::onAssignTruck}},
	btnUnassignTruck{"Remove Truck", {this, &MineOperationsWindow::onUnassignTruck}}
{
	size({375, 270});

	// Set up GUI Layout
	add(btnIdle, {10, 230});
	btnIdle.type(Button::Type::Toggle);
	btnIdle.size({60, 30});

	add(btnExtendShaft, {72, 230});
	btnExtendShaft.size({100, 30});

	add(btnOkay, {mRect.size.x - 70, 230});
	btnOkay.size({60, 30});

	add(btnAssignTruck, {mRect.size.x - 85, 115});
	btnAssignTruck.size({80, 20});

	add(btnUnassignTruck, {mRect.size.x - 170, 115});
	btnUnassignTruck.size({80, 20});

	// ORE TOGGLE BUTTONS
	add(chkResources[0], {148, 140});
	add(chkResources[1], {259, 140});
	add(chkResources[2], {148, 160});
	add(chkResources[3], {259, 160});
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

	const auto enabledBits = mFacility->mine()->miningEnabled();
	chkResources[0].checked(enabledBits[0]);
	chkResources[1].checked(enabledBits[1]);
	chkResources[2].checked(enabledBits[2]);
	chkResources[3].checked(enabledBits[3]);

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
	mFacility->forceIdle(btnIdle.isPressed());
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
	mFacility->mine()->miningEnabled(Mine::OreType::CommonMetals, chkResources[0].checked());
}


void MineOperationsWindow::onCheckBoxCommonMineralsChange()
{
	mFacility->mine()->miningEnabled(Mine::OreType::CommonMinerals, chkResources[1].checked());
}


void MineOperationsWindow::onCheckBoxRareMetalsChange()
{
	mFacility->mine()->miningEnabled(Mine::OreType::RareMetals, chkResources[2].checked());
}


void MineOperationsWindow::onCheckBoxRareMineralsChange()
{
	mFacility->mine()->miningEnabled(Mine::OreType::RareMinerals, chkResources[3].checked());
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

	const auto origin = mRect.position;
	renderer.drawImage(mUiIcon, origin + NAS2D::Vector{10, 30});

	const auto& mineYield = MINE_YIELD_TRANSLATION.at(mFacility->mine()->productionRate());
	drawLabelAndValue(origin + NAS2D::Vector{148, 30}, "Mine Yield: ", mineYield);

	const std::string statusString =
		mFacility->extending() ? "Digging New Level" :
		mFacility->mine()->exhausted() ? "Exhausted" :
		mFacility->stateDescription();

	drawLabelAndValue(origin + NAS2D::Vector{148, 45}, "Status: ", statusString);

	if (mFacility->extending())
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
	const auto width = mRect.size.x;
	renderer.drawText(mFontBold, "Remaining Resources", origin + NAS2D::Vector{10, 164}, NAS2D::Color::White);

	mPanel.draw(renderer, NAS2D::Rectangle{origin + NAS2D::Vector{10, 180}, {width - 20, 40}});

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
