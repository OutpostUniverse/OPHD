#include "MineOperationsWindow.h"

#include "TextRender.h"

#include "../CacheImage.h"
#include "../OreDepositYieldString.h"
#include "../Resources.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../TruckAvailability.h"
#include "../MapObjects/Structures/MineFacility.h"

#include <libOPHD/MapObjects/OreDeposit.h>

#include <libControls/LoadRectangleSkin.h>

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>


namespace
{
	const auto truckAvailabilityOffset = NAS2D::Vector{148, 80};
	const auto truckButtonOffset = truckAvailabilityOffset + NAS2D::Vector{0, 35};
	const auto truckButtonSize = NAS2D::Vector{128, 25};
}


MineOperationsWindow::MineOperationsWindow() :
	Window{constants::WindowMineOperations},
	mFont{Control::getDefaultFont()},
	mFontBold{Control::getDefaultFontBold()},
	mUiIcon{imageCache.load("ui/interface/mine.png")},
	mIcons{imageCache.load("ui/icons.png")},
	mPanel{loadRectangleSkin("ui/skin/textbox_normal")},
	btnIdle{"Idle", {this, &MineOperationsWindow::onIdle}},
	btnExtendShaft{"Dig New Level", {this, &MineOperationsWindow::onExtendShaft}},
	btnOkay{"Close", {this, &MineOperationsWindow::onOkay}},
	btnAssignTruck{"Add Truck", {this, &MineOperationsWindow::onAssignTruck}},
	btnUnassignTruck{"Remove Truck", {this, &MineOperationsWindow::onUnassignTruck}}
{
	size({420, 300});

	// Set up GUI Layout
	btnIdle.type(Button::Type::Toggle);
	btnIdle.size({60, 30});
	btnExtendShaft.size({120, 30});
	btnOkay.size({60, 30});

	const auto buttonBottomRowY = mRect.size.y - btnIdle.size().y - 10;
	add(btnIdle, {10, buttonBottomRowY});
	add(btnExtendShaft, {76, buttonBottomRowY});
	add(btnOkay, {mRect.size.x - 70, buttonBottomRowY});

	btnAssignTruck.size(truckButtonSize);
	btnUnassignTruck.size(truckButtonSize);

	add(btnAssignTruck, truckButtonOffset);
	add(btnUnassignTruck, {truckButtonOffset.x + truckButtonSize.x + constants::Margin, truckButtonOffset.y});
}


void MineOperationsWindow::hide()
{
	Window::hide();
	mFacility = nullptr;
}


void MineOperationsWindow::mineFacility(MineFacility* facility)
{
	mFacility = facility;
	if (!mFacility) { return; }

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


void MineOperationsWindow::updateTruckAvailability()
{
	mAvailableTrucks = getTruckAvailability();
}


void MineOperationsWindow::drawClientArea(NAS2D::Renderer& renderer) const
{
	const auto origin = mRect.position;
	renderer.drawImage(mUiIcon, origin + NAS2D::Vector{10, 30});

	const auto& mineYield = oreDepositYieldEnumToString(mFacility->oreDeposit().yield());
	drawLabelAndValue(origin + NAS2D::Vector{148, 30}, "Mine Yield: ", mineYield);

	const std::string statusString =
		mFacility->extending() ? "Digging New Level" :
		mFacility->oreDeposit().isExhausted() ? "Exhausted" :
		mFacility->stateDescription();

	drawLabelAndValue(origin + NAS2D::Vector{148, 45}, "Status: ", statusString);

	if (mFacility->extending())
	{
		const auto extensionTimeRemaining = std::to_string(mFacility->digTimeRemaining());
		drawLabelAndValue(origin + NAS2D::Vector{148, 60}, "Turns Remaining: ", extensionTimeRemaining);
	}

	const auto mineDepth = std::to_string(mFacility->oreDeposit().digDepth());
	drawLabelAndValue(origin + NAS2D::Vector{300, 30}, "Depth: ", mineDepth);

	// TRUCK ASSIGNMENT
	const auto truckAssignmentOrigin = origin + truckAvailabilityOffset;
	renderer.drawText(mFontBold, "Trucks", truckAssignmentOrigin, NAS2D::Color::White);
	drawLabelAndValue(truckAssignmentOrigin + NAS2D::Vector{0, 15}, "Assigned: ", std::to_string(mFacility->assignedTrucks()));
	drawLabelAndValue(truckAssignmentOrigin + NAS2D::Vector{137, 15}, "Available: ", std::to_string(mAvailableTrucks));

	// REMAINING ORE PANEL
	const auto tableSize = NAS2D::Vector{mRect.size.x - 20, 40};
	const auto cellSize = NAS2D::Vector{tableSize.x / 4, tableSize.y / 2};
	const auto tableOrigin = btnIdle.position() - NAS2D::Vector{0, tableSize.y + 10};
	const auto tableTitleOrigin = tableOrigin - NAS2D::Vector{0, mFontBold.height() + constants::MarginTight};

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
