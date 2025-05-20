#include "MineOperationsWindow.h"

#include "StringTable.h"
#include "TextRender.h"

#include "../Cache.h"
#include "../OreDepositYieldString.h"
#include "../Resources.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../StructureManager.h"
#include "../TruckAvailability.h"
#include "../MapObjects/OreDeposit.h"
#include "../MapObjects/Structures/MineFacility.h"
#include "../MapObjects/Structures/Warehouse.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

using namespace NAS2D;


MineOperationsWindow::MineOperationsWindow() :
	Window{constants::WindowMineOperations},
	mFont{Control::getDefaultFont()},
	mFontBold{Control::getDefaultFontBold()},
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
	size({376, 270});

	// Set up GUI Layout
	btnIdle.type(Button::Type::Toggle);
	btnIdle.size({60, 30});
	btnExtendShaft.size({100, 30});
	btnOkay.size({60, 30});

	const auto buttonBottomRowY = mRect.size.y - btnIdle.size().y - 10;
	add(btnIdle, {10, buttonBottomRowY});
	add(btnExtendShaft, {72, buttonBottomRowY});
	add(btnOkay, {mRect.size.x - 70, buttonBottomRowY});

	btnAssignTruck.size({105, 20});
	btnUnassignTruck.size({105, 20});

	add(btnAssignTruck, {mRect.size.x - btnAssignTruck.size().x - 10, 115});
	add(btnUnassignTruck, {148, 115});

	// ORE TOGGLE BUTTONS
	const auto checkBoxOrigin = NAS2D::Vector{148, 140};
	const auto checkBoxOffset = NAS2D::Vector{122, 20};
	add(chkResources[0], checkBoxOrigin);
	add(chkResources[1], checkBoxOrigin + NAS2D::Vector{0, checkBoxOffset.y});
	add(chkResources[2], checkBoxOrigin + NAS2D::Vector{checkBoxOffset.x, 0});
	add(chkResources[3], checkBoxOrigin + checkBoxOffset);
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

	const auto enabledBits = mFacility->oreDeposit().miningEnabled();
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
	mFacility->oreDeposit().miningEnabled(OreDeposit::OreType::CommonMetals, chkResources[0].checked());
}


void MineOperationsWindow::onCheckBoxCommonMineralsChange()
{
	mFacility->oreDeposit().miningEnabled(OreDeposit::OreType::CommonMinerals, chkResources[1].checked());
}


void MineOperationsWindow::onCheckBoxRareMetalsChange()
{
	mFacility->oreDeposit().miningEnabled(OreDeposit::OreType::RareMetals, chkResources[2].checked());
}


void MineOperationsWindow::onCheckBoxRareMineralsChange()
{
	mFacility->oreDeposit().miningEnabled(OreDeposit::OreType::RareMinerals, chkResources[3].checked());
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

	const auto& mineYield = oreDepositYieldEnumToString(mFacility->oreDeposit().yield());
	drawLabelAndValue(origin + NAS2D::Vector{148, 30}, "Mine Yield: ", mineYield);

	const std::string statusString =
		mFacility->extending() ? "Digging New Level" :
		mFacility->oreDeposit().exhausted() ? "Exhausted" :
		mFacility->stateDescription();

	drawLabelAndValue(origin + NAS2D::Vector{148, 45}, "Status: ", statusString);

	if (mFacility->extending())
	{
		const auto extensionTimeRemaining = std::to_string(mFacility->digTimeRemaining());
		drawLabelAndValue(origin + NAS2D::Vector{148, 60}, "Turns Remaining: ", extensionTimeRemaining);
	}

	const auto mineDepth = std::to_string(mFacility->oreDeposit().depth());
	drawLabelAndValue(origin + NAS2D::Vector{300, 30}, "Depth: ", mineDepth);

	// TRUCK ASSIGNMENT
	renderer.drawText(mFontBold, "Trucks", origin + NAS2D::Vector{148, 80}, NAS2D::Color::White);
	drawLabelAndValue(origin + NAS2D::Vector{148, 95}, "Assigned: ", std::to_string(mFacility->assignedTrucks()));
	drawLabelAndValue(origin + NAS2D::Vector{260, 95}, "Available: ", std::to_string(mAvailableTrucks));

	// REMAINING ORE PANEL
	const auto tableSize = NAS2D::Vector{mRect.size.x - 20, 40};
	const auto cellSize = NAS2D::Vector{tableSize.x / 4, tableSize.y / 2};
	const auto tableOrigin = btnIdle.position() - NAS2D::Vector{0, tableSize.y + 10};
	const auto tableTitleOrigin = tableOrigin - NAS2D::Vector{0, mFontBold.height() - 1};

	renderer.drawText(mFontBold, "Remaining Resources", tableTitleOrigin, NAS2D::Color::White);

	mPanel.draw(renderer, NAS2D::Rectangle{tableOrigin, tableSize});

	const auto dividerLineColor = NAS2D::Color{22, 22, 22};

	const auto rowOrigin = tableOrigin + NAS2D::Vector{1, cellSize.y};
	renderer.drawLine(rowOrigin, rowOrigin + NAS2D::Vector{tableSize.x - 2, 0}, dividerLineColor);

	for (int i = 1; i < 4; ++i)
	{
		const auto columnOrigin = tableOrigin + NAS2D::Vector{cellSize.x * i, 1};
		renderer.drawLine(columnOrigin, columnOrigin + NAS2D::Vector{0, tableSize.y - 2}, dividerLineColor);
	}

	const auto availableResources = mFacility->oreDeposit().availableResources();
	const std::array resources
	{
		std::tuple{ResourceImageRectsOre[0], availableResources.resources[0]},
		std::tuple{ResourceImageRectsOre[1], availableResources.resources[1]},
		std::tuple{ResourceImageRectsOre[2], availableResources.resources[2]},
		std::tuple{ResourceImageRectsOre[3], availableResources.resources[3]}
	};

	auto columnOrigin = tableOrigin + NAS2D::Vector{0, 1};
	for (const auto& [iconRect, resourceCount] : resources)
	{
		const auto resourceCountString = std::to_string(resourceCount);
		renderer.drawSubImage(mIcons, columnOrigin + (cellSize - iconRect.size) / 2, iconRect);
		const auto textPosition = columnOrigin + NAS2D::Vector{0, cellSize.y} + (cellSize - mFont.size(resourceCountString)) / 2;
		renderer.drawText(mFont, resourceCountString, textPosition, NAS2D::Color::White);
		columnOrigin.x += cellSize.x;
	}
}
