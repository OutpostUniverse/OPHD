#include "MineReport.h"

#include "../TextRender.h"
#include "../ProgressBar.h"

#include "../../Constants/Numbers.h"
#include "../../Constants/Strings.h"
#include "../../Constants/UiConstants.h"

#include "../../Cache.h"
#include "../../Resources.h"
#include "../../StructureManager.h"
#include "../../TruckAvailability.h"
#include "../../ProductionCost.h"

#include "../../States/Route.h"

#include "../../Map/Tile.h"
#include "../../MapObjects/OreDeposit.h"
#include "../../MapObjects/Structures/MineFacility.h"

#include <libControls/Layout.h>

#include <NAS2D/StringFrom.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>
#include <cfloat>
#include <map>


using namespace NAS2D;


namespace
{
	std::string getStructureDescription(const Structure& structure)
	{
		const auto& structureManager = NAS2D::Utility<StructureManager>::get();
		const auto& surfaceLocation = structureManager.tileFromStructure(&structure).xy();
		return structure.name() + " at " + NAS2D::stringFrom(surfaceLocation);
	}
}


MineReport::MineReport() :
	font{Control::getDefaultFont()},
	fontBold{Control::getDefaultFontBold()},
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	mineFacility{imageCache.load("ui/interface/mine.png")},
	uiIcons{imageCache.load("ui/icons.png")},
	btnShowAll{"All", {this, &MineReport::onShowAll}},
	btnShowActive{"Active", {this, &MineReport::onShowActive}},
	btnShowIdle{"Idle", {this, &MineReport::onShowIdle}},
	btnShowTappedOut{"Tapped Out", {this, &MineReport::onShowTappedOut}},
	btnShowDisabled{"Disabled", {this, &MineReport::onShowDisabled}},
	btnIdle{constants::Idle, {this, &MineReport::onIdle}},
	btnDigNewLevel{"Dig New Level", {this, &MineReport::onDigNewLevel}},
	btnTakeMeThere{constants::TakeMeThere, {this, &MineReport::onTakeMeThere}},
	btnAddTruck{constants::AddTruck, {this, &MineReport::onAddTruck}},
	btnRemoveTruck{constants::RemoveTruck, {this, &MineReport::onRemoveTruck}},
	chkResources{{
		{"Mine " + ResourceNamesRefined[0], {this, &MineReport::onCheckBoxCommonMetalsChange}},
		{"Mine " + ResourceNamesRefined[1], {this, &MineReport::onCheckBoxCommonMineralsChange}},
		{"Mine " + ResourceNamesRefined[2], {this, &MineReport::onCheckBoxRareMetalsChange}},
		{"Mine " + ResourceNamesRefined[3], {this, &MineReport::onCheckBoxRareMineralsChange}}
	}},
	mSelectedFacility{nullptr},
	mAvailableTrucks{0}
{
	auto buttonOffset = NAS2D::Vector{10, 10};
	const auto buttons = std::array{&btnShowAll, &btnShowActive, &btnShowIdle, &btnShowTappedOut, &btnShowDisabled};
	for (auto button : buttons)
	{
		button->size({94, 20});
		button->type(Button::Type::Toggle);
		add(*button, buttonOffset);
		buttonOffset.x += button->size().x + constants::MarginTight;
	}

	btnShowAll.toggle(true);

	lstMineFacilities.selectionChanged().connect({this, &MineReport::onMineFacilitySelectionChange});
	add(lstMineFacilities, {10, 40});

	// DETAIL PANE
	btnIdle.type(Button::Type::Toggle);
	btnIdle.size({140, 30});

	btnDigNewLevel.size({140, 30});
	btnTakeMeThere.size({140, 30});

	add(btnIdle, {0, 40});
	add(btnDigNewLevel, {0, 75});
	add(btnTakeMeThere, {0, 110});

	const auto checkBoxOriginY = 170 + fontMediumBold.height() + 10 + 10;
	const auto resourceNameHeight = std::max({ResourceImageRectsOre[0].size.y, fontBold.height(), chkResources[0].size().y});
	const auto resourceProgressBarHeight = std::max(25, fontBold.height() + constants::MarginTight * 2);
	const auto checkBoxSpacingY = resourceNameHeight + resourceProgressBarHeight + constants::Margin + 23;
	add(chkResources[0], {0, checkBoxOriginY});
	add(chkResources[1], {0, checkBoxOriginY + checkBoxSpacingY});
	add(chkResources[2], {0, checkBoxOriginY + checkBoxSpacingY * 2});
	add(chkResources[3], {0, checkBoxOriginY + checkBoxSpacingY * 3});

	// Truck Management Pane
	btnAddTruck.size({140, 30});
	btnRemoveTruck.size({140, 30});

	add(btnAddTruck, {0, 215});
	add(btnRemoveTruck, {0, 250});

	fillLists();
}


void MineReport::selectStructure(Structure* structure)
{
	lstMineFacilities.setSelected(structure);
}


void MineReport::clearSelected()
{
	lstMineFacilities.clearSelected();
	mSelectedFacility = nullptr;
}


void MineReport::refresh()
{
	onShowAll();
}


void MineReport::fillLists()
{
	lstMineFacilities.clear();
	const auto& structureManager = NAS2D::Utility<StructureManager>::get();
	for (auto* facility : structureManager.getStructures<MineFacility>())
	{
		lstMineFacilities.addItem(facility);
		lstMineFacilities.last()->text = getStructureDescription(*facility);
	}

	lstMineFacilities.setSelected(mSelectedFacility);
	mAvailableTrucks = getTruckAvailability();
}


void MineReport::onResize()
{
	Control::onResize();

	lstMineFacilities.size({area().center().x - 20, area().size.y - 51});

	const auto buttonPositionX = area().size.x - 150;
	btnIdle.position({buttonPositionX, btnIdle.position().y});
	btnDigNewLevel.position({buttonPositionX, btnDigNewLevel.position().y});
	btnTakeMeThere.position({buttonPositionX, btnTakeMeThere.position().y});

	auto& renderer = NAS2D::Utility<Renderer>::get();
	btnAddTruck.position({buttonPositionX, renderer.size().y - 130});
	btnRemoveTruck.position({buttonPositionX, renderer.size().y - 95});

	const auto checkBoxes = std::vector<Control*>{&chkResources[0], &chkResources[1], &chkResources[2], &chkResources[3]};
	const auto maxCheckBoxSize = maxSize(checkBoxes);
	const auto checkBoxPositionX = area().size.x - maxCheckBoxSize.x - 10;
	setPositionX(checkBoxes, checkBoxPositionX);
}


void MineReport::onVisibilityChange(bool /*visible*/)
{
	onManagementButtonsVisibilityChange();
}


void MineReport::onManagementButtonsVisibilityChange()
{
	bool isVisible = visible() && mSelectedFacility;

	btnIdle.visible(isVisible);
	btnDigNewLevel.visible(isVisible);
	btnTakeMeThere.visible(isVisible);

	bool isTruckButtonVisible = isVisible &&
		!(mSelectedFacility->destroyed() || mSelectedFacility->underConstruction());

	btnAddTruck.visible(isTruckButtonVisible);
	btnRemoveTruck.visible(isTruckButtonVisible);

	for (auto& chkResource : chkResources)
	{
		chkResource.visible(isVisible);
	}
}


void MineReport::onFilterButtonClicked()
{
	btnShowAll.toggle(false);
	btnShowActive.toggle(false);
	btnShowIdle.toggle(false);
	btnShowTappedOut.toggle(false);
	btnShowDisabled.toggle(false);
}


void MineReport::onShowAll()
{
	onFilterButtonClicked();
	btnShowAll.toggle(true);

	fillLists();
}


void MineReport::onShowActive()
{
	onFilterButtonClicked();
	btnShowActive.toggle(true);
}


void MineReport::onShowIdle()
{
	onFilterButtonClicked();
	btnShowIdle.toggle(true);
}


void MineReport::onShowTappedOut()
{
	onFilterButtonClicked();
	btnShowTappedOut.toggle(true);
}


void MineReport::onShowDisabled()
{
	onFilterButtonClicked();
	btnShowDisabled.toggle(true);
}


void MineReport::onMineFacilitySelectionChange()
{
	mSelectedFacility = dynamic_cast<MineFacility*>(lstMineFacilities.selectedStructure());

	onManagementButtonsVisibilityChange();

	if (!mSelectedFacility) { return; }

	const auto& facility = *mSelectedFacility;
	btnIdle.toggle(facility.isIdle());
	btnIdle.enabled(facility.operational() || facility.isIdle());

	btnDigNewLevel.toggle(facility.extending());
	btnDigNewLevel.enabled(facility.canExtend() && (facility.operational() || facility.isIdle()));

	const auto enabledBits = facility.oreDeposit().miningEnabled();
	chkResources[0].checked(enabledBits[0]);
	chkResources[1].checked(enabledBits[1]);
	chkResources[2].checked(enabledBits[2]);
	chkResources[3].checked(enabledBits[3]);
}


void MineReport::onIdle()
{
	mSelectedFacility->forceIdle(btnIdle.isPressed());
}


void MineReport::onDigNewLevel()
{
	auto& facility = *mSelectedFacility;
	facility.extend();

	btnDigNewLevel.toggle(facility.extending());
	btnDigNewLevel.enabled(facility.canExtend());
}


void MineReport::onTakeMeThere()
{
	mTakeMeThereSignal(mSelectedFacility);
}


void MineReport::onCheckBoxCommonMetalsChange()
{
	mSelectedFacility->oreDeposit().miningEnabled(OreDeposit::OreType::CommonMetals, chkResources[0].checked());
}


void MineReport::onCheckBoxCommonMineralsChange()
{
	mSelectedFacility->oreDeposit().miningEnabled(OreDeposit::OreType::CommonMinerals, chkResources[1].checked());
}


void MineReport::onCheckBoxRareMetalsChange()
{
	mSelectedFacility->oreDeposit().miningEnabled(OreDeposit::OreType::RareMetals, chkResources[2].checked());
}


void MineReport::onCheckBoxRareMineralsChange()
{
	mSelectedFacility->oreDeposit().miningEnabled(OreDeposit::OreType::RareMinerals, chkResources[3].checked());
}


void MineReport::onAddTruck()
{
	if (!mSelectedFacility) { return; }

	auto& facility = *mSelectedFacility;

	if (facility.assignedTrucks() == facility.maxTruckCount()) { return; }

	if (pullTruckFromInventory())
	{
		facility.addTruck();
		mAvailableTrucks = getTruckAvailability();
	}
}


void MineReport::onRemoveTruck()
{
	if (!mSelectedFacility) { return; }

	auto& facility = *mSelectedFacility;

	if (facility.assignedTrucks() == 1) { return; }

	if (pushTruckIntoInventory())
	{
		facility.removeTruck();
		mAvailableTrucks = getTruckAvailability();
	}
}


void MineReport::drawMineFacilityPane(const NAS2D::Point<int>& origin)
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(mineFacility, origin);
	const auto text = lstMineFacilities.isItemSelected() ? getStructureDescription(*lstMineFacilities.selectedStructure()) : "";
	renderer.drawText(fontBigBold, text, origin + NAS2D::Vector{0, -33}, constants::PrimaryTextColor);

	renderer.drawText(fontMediumBold, "Status", origin + NAS2D::Vector{138, 0}, constants::PrimaryTextColor);

	const auto& facility = *mSelectedFacility;
	const bool isStatusHighlighted = facility.disabled() || facility.destroyed();
	const auto statusPosition = btnIdle.position() - NAS2D::Vector{fontMedium.width(facility.stateDescription()) + 5, 0};
	renderer.drawText(fontMedium, facility.stateDescription(), statusPosition, (isStatusHighlighted ? NAS2D::Color::Red : constants::PrimaryTextColor));
}


void MineReport::drawOreProductionPane(const NAS2D::Point<int>& origin)
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawText(fontMediumBold, "Ore Production", origin, constants::PrimaryTextColor);
	const auto lineOffset = NAS2D::Vector{0, fontMediumBold.height() + 1};
	const auto lineOrigin = origin + lineOffset;
	renderer.drawLine(lineOrigin, lineOrigin + NAS2D::Vector{renderer.size().x - lineOrigin.x - 10, 0}, constants::PrimaryTextColor, 1);

	const auto& mine = mSelectedFacility->oreDeposit();
	const auto oreAvailable = mine.availableResources();
	const auto oreTotalYield = mine.totalYield();

	auto resourceOffset = lineOffset + NAS2D::Vector{0, 1 + constants::Margin + 2};
	const auto progressBarSize = NAS2D::Vector{renderer.size().x - origin.x - 10, std::max(25, fontBold.height() + constants::MarginTight * 2)};
	for (size_t i = 0; i < 4; ++i)
	{
		const auto resourceIconPosition = origin + resourceOffset;
		renderer.drawSubImage(uiIcons, resourceIconPosition, ResourceImageRectsOre[i]);
		const auto resourceNameOffset = NAS2D::Vector{ResourceImageRectsOre[i].size.x + constants::MarginTight + 2, 0};
		renderer.drawText(fontBold, ResourceNamesOre[i], resourceIconPosition + resourceNameOffset, constants::PrimaryTextColor);

		const auto resourceNameHeight = std::max({ResourceImageRectsOre[i].size.y, fontBold.height(), chkResources[i].size().y});
		const auto progressBarPosition = resourceIconPosition + NAS2D::Vector{0, resourceNameHeight + constants::MarginTight + 2};
		const auto progressBarArea = NAS2D::Rectangle{progressBarPosition, progressBarSize};
		drawProgressBar(
			oreAvailable.resources[i],
			oreTotalYield.resources[i],
			progressBarArea
		);

		const std::string str = std::to_string(oreAvailable.resources[i]) + " of " + std::to_string(oreTotalYield.resources[i]) + " Remaining";
		const auto strOffset = (progressBarSize - fontBold.size(str)) / 2;
		renderer.drawText(fontBold, str, progressBarPosition + strOffset);

		resourceOffset.y += resourceNameHeight + progressBarSize.y + constants::Margin + 23;
	}
}


void MineReport::drawTruckManagementPane(const NAS2D::Point<int>& origin)
{
	const auto& miningFacility = *mSelectedFacility;

	if (miningFacility.destroyed() ||
		miningFacility.underConstruction())
	{
		return;
	}

	auto& renderer = Utility<Renderer>::get();
	renderer.drawText(fontMediumBold, "Trucks & Routing", origin, constants::PrimaryTextColor);
	renderer.drawLine(origin + NAS2D::Vector{0, 21}, NAS2D::Point{renderer.size().x - 10, origin.y + 21}, constants::PrimaryTextColor, 1);

	renderer.drawText(fontBold, "Trucks Assigned to Facility", origin + NAS2D::Vector{0, 30}, constants::PrimaryTextColor);

	const auto labelWidth = btnAddTruck.position().x - origin.x - 10;
	drawLabelAndValueRightJustify(
		origin + NAS2D::Vector{0, 30},
		labelWidth,
		"Trucks Assigned to Facility",
		std::to_string(miningFacility.assignedTrucks()),
		constants::PrimaryTextColor
	);
	drawLabelAndValueRightJustify(
		origin + NAS2D::Vector{0, 45},
		labelWidth,
		"Trucks Available in Storage",
		std::to_string(mAvailableTrucks),
		constants::PrimaryTextColor
	);

	const auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	bool routeAvailable = routeTable.find(mSelectedFacility) != routeTable.end();

	if (miningFacility.operational() || miningFacility.isIdle())
	{
		drawLabelAndValueRightJustify(
			origin + NAS2D::Vector{0, 65},
			labelWidth,
			"Route Available",
			routeAvailable ? "Yes" : "No",
			routeAvailable ? constants::PrimaryTextColor : NAS2D::Color::Red
		);

		if (routeAvailable)
		{
			drawTruckHaulInfo(origin + NAS2D::Vector{0, 80});
		}
	}
}


void MineReport::drawTruckHaulInfo(const NAS2D::Point<int>& origin)
{
	auto& renderer = Utility<Renderer>::get();

	const auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	const auto& route = routeTable.at(mSelectedFacility);
	drawLabelAndValueRightJustify(
		origin,
		btnAddTruck.position().x - origin.x - 10,
		"Route Cost",
		std::to_string(route.cost).substr(0, std::to_string(route.cost).find(".") + 3), // hack-ish and probably slow, this could be cached
		constants::PrimaryTextColor
	);


	const float routeCost = std::clamp(route.cost, 1.0f, FLT_MAX);
	const int totalOreMovement = static_cast<int>(constants::ShortestPathTraversalCount / routeCost) * mSelectedFacility->assignedTrucks();
	const int oreMovementLabelWidth = renderer.size().x - origin.x - 10;
	const int oreMovementPart = totalOreMovement / 4;
	const int oreLabelWidth = (oreMovementLabelWidth - 10) / 2;

	const NAS2D::Rectangle<int> tableRect({{origin.x - 2, origin.y + 18}, {oreMovementLabelWidth + 5, 47}});

	renderer.drawBoxFilled(tableRect, {0, 0, 0, 100});
	renderer.drawBox(tableRect, constants::PrimaryTextColor);

	renderer.drawLine(origin + NAS2D::Vector{0, 34}, origin + NAS2D::Vector{oreMovementLabelWidth, 34}, constants::PrimaryTextColor);
	renderer.drawLine(origin + NAS2D::Vector{0, 50}, origin + NAS2D::Vector{oreMovementLabelWidth, 50}, constants::PrimaryTextColor);
	renderer.drawLine(origin + NAS2D::Vector{oreLabelWidth + 5, 37}, origin + NAS2D::Vector{oreLabelWidth + 5, 63}, constants::PrimaryTextColor);

	drawLabelAndValueRightJustify(
		origin + NAS2D::Vector{0, 20},
		oreMovementLabelWidth,
		"Total Haul Capacity per Turn",
		std::to_string(totalOreMovement),
		constants::PrimaryTextColor
	);

	drawLabelAndValueRightJustify(
		origin + NAS2D::Vector{0, 35},
		oreLabelWidth,
		ResourceNamesOre[0] + " Haul Capacity",
		std::to_string(oreMovementPart),
		constants::PrimaryTextColor
	);

	drawLabelAndValueRightJustify(
		origin + NAS2D::Vector{oreLabelWidth + 10, 35},
		oreLabelWidth,
		ResourceNamesOre[1] + " Haul Capacity",
		std::to_string(oreMovementPart),
		constants::PrimaryTextColor
	);

	drawLabelAndValueRightJustify(
		origin + NAS2D::Vector{0, 50},
		oreLabelWidth,
		ResourceNamesOre[2] + " Haul Capacity",
		std::to_string(oreMovementPart),
		constants::PrimaryTextColor
	);

	drawLabelAndValueRightJustify(
		origin + NAS2D::Vector{oreLabelWidth + 10, 50},
		oreLabelWidth,
		ResourceNamesOre[3] + " Haul Capacity",
		std::to_string(oreMovementPart + (totalOreMovement % 4)),
		constants::PrimaryTextColor
	);
}


void MineReport::update()
{
	if (!visible()) { return; }

	auto& renderer = Utility<Renderer>::get();

	const auto startPoint = NAS2D::Point{area().center().x , area().position.y + 10};

	renderer.drawLine(startPoint, startPoint + NAS2D::Vector{0, area().size.y - 20}, constants::PrimaryTextColor);

	if (mSelectedFacility)
	{
		drawMineFacilityPane(startPoint + NAS2D::Vector{10, 30});
		drawOreProductionPane(startPoint + NAS2D::Vector{10, 170});
		drawTruckManagementPane(startPoint + NAS2D::Vector{10, renderer.size().y - 214});
	}

	UIContainer::update();
}
