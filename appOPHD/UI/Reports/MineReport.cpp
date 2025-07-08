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

#include "../../Map/Route.h"
#include "../../Map/Tile.h"
#include "../../MapObjects/Structures/MineFacility.h"

#include <libOPHD/MapObjects/OreDeposit.h>

#include <NAS2D/StringFrom.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>
#include <cfloat>
#include <map>
#include <algorithm>


using namespace NAS2D;


namespace
{
	std::string getStructureDescription(const Structure& structure)
	{
		const auto& structureManager = NAS2D::Utility<StructureManager>::get();
		const auto& surfaceLocation = structureManager.tileFromStructure(&structure).xy();
		return structure.name() + " at " + NAS2D::stringFrom(surfaceLocation);
	}

	bool hasRoute(MineFacility* mineFacility)
	{
		const auto& routeTable = NAS2D::Utility<std::map<const MineFacility*, Route>>::get();
		return routeTable.find(mineFacility) != routeTable.end();
	}

	float getRouteCost(MineFacility* mineFacility)
	{
		const auto& routeTable = NAS2D::Utility<std::map<const MineFacility*, Route>>::get();
		if (routeTable.find(mineFacility) == routeTable.end())
		{
			return FLT_MAX;
		}
		const auto& route = routeTable.at(mineFacility);
		return std::clamp(route.cost, 1.0f, FLT_MAX);
	}

	int getOreHaulCapacity(MineFacility* mineFacility)
	{
		const float routeCost = getRouteCost(mineFacility);
		return static_cast<int>(constants::ShortestPathTraversalCount / routeCost) * mineFacility->assignedTrucks();
	}

	std::string formatRouteCost(float routeCost)
	{
		const auto routeCostString = std::to_string(routeCost);
		return routeCostString.substr(0, routeCostString.find(".") + 3);
	}
}


MineReport::MineReport(TakeMeThereDelegate takeMeThereHandler) :
	mTakeMeThereHandler{takeMeThereHandler},
	font{Control::getDefaultFont()},
	fontBold{Control::getDefaultFontBold()},
	fontMedium{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	fontMediumBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	fontBigBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryHuge)},
	mineFacilityImage{imageCache.load("ui/interface/mine.png")},
	uiIcons{imageCache.load("ui/icons.png")},
	btnShowAll{"All", {this, &MineReport::onShowAll}},
	btnShowActive{"Active", {this, &MineReport::onShowActive}},
	btnShowIdle{"Idle", {this, &MineReport::onShowIdle}},
	btnShowExhausted{"Exhausted", {this, &MineReport::onShowExhausted}},
	btnShowDisabled{"Disabled", {this, &MineReport::onShowDisabled}},
	btnIdle{constants::Idle, {this, &MineReport::onIdle}},
	btnDigNewLevel{"Dig New Level", {this, &MineReport::onDigNewLevel}},
	btnTakeMeThere{constants::TakeMeThere, {this, &MineReport::onTakeMeThere}},
	btnAddTruck{constants::AddTruck, {this, &MineReport::onAddTruck}},
	btnRemoveTruck{constants::RemoveTruck, {this, &MineReport::onRemoveTruck}},
	lstMineFacilities{{this, &MineReport::onMineFacilitySelectionChange}},
	mSelectedFacility{nullptr},
	mAvailableTrucks{0}
{
	auto buttonOffset = NAS2D::Vector{10, 10};
	const auto buttonSize = NAS2D::Vector{94, 26};
	const auto buttons = std::array{&btnShowAll, &btnShowActive, &btnShowIdle, &btnShowExhausted, &btnShowDisabled};
	for (auto button : buttons)
	{
		button->size(buttonSize);
		button->type(Button::Type::Toggle);
		add(*button, buttonOffset);
		buttonOffset.x += button->size().x + constants::MarginTight;
	}

	btnShowAll.toggle(true);

	add(lstMineFacilities, {10, 10 + buttonOffset.y + buttonSize.y});

	// DETAIL PANE
	btnIdle.type(Button::Type::Toggle);
	btnIdle.size({140, 30});
	btnDigNewLevel.size({140, 30});
	btnTakeMeThere.size({140, 30});
	btnAddTruck.size({140, 30});
	btnRemoveTruck.size({140, 30});

	add(btnIdle, {0, 40});
	add(btnDigNewLevel, {0, 75});
	add(btnTakeMeThere, {0, 110});
	add(btnAddTruck, {0, 145});
	add(btnRemoveTruck, {0, 180});

	fillLists();
}


bool MineReport::canView(const Structure& structure)
{
	return dynamic_cast<const MineFacility*>(&structure) || structure.isSmelter();
}


void MineReport::selectStructure(Structure& structure)
{
	lstMineFacilities.setSelected(&structure);
}


void MineReport::clearSelected()
{
	lstMineFacilities.clearSelected();
	mSelectedFacility = nullptr;
}


void MineReport::fillLists()
{
	lstMineFacilities.clear();
	const auto& structureManager = NAS2D::Utility<StructureManager>::get();
	for (auto* mineFacility : structureManager.getStructures<MineFacility>())
	{
		lstMineFacilities.addItem(mineFacility);
		lstMineFacilities.last()->text = getStructureDescription(*mineFacility);
	}

	lstMineFacilities.setSelected(mSelectedFacility);
	mAvailableTrucks = getTruckAvailability();
}


void MineReport::refresh()
{
	onShowAll();
}


void MineReport::onResize()
{
	Control::onResize();

	const auto centerX = area().center().x;
	lstMineFacilities.size({centerX - 20, area().crossYPoint().y - lstMineFacilities.position().y - 10});

	const auto buttonPositionX = area().size.x - 150;
	btnIdle.position({buttonPositionX, btnIdle.position().y});
	btnDigNewLevel.position({buttonPositionX, btnDigNewLevel.position().y});
	btnTakeMeThere.position({buttonPositionX, btnTakeMeThere.position().y});
	btnAddTruck.position({buttonPositionX, btnAddTruck.position().y});
	btnRemoveTruck.position({buttonPositionX, btnRemoveTruck.position().y});
}


void MineReport::onVisibilityChange(bool visible)
{
	Report::onVisibilityChange(visible);

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
}


void MineReport::onFilterButtonClicked()
{
	btnShowAll.toggle(false);
	btnShowActive.toggle(false);
	btnShowIdle.toggle(false);
	btnShowExhausted.toggle(false);
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


void MineReport::onShowExhausted()
{
	onFilterButtonClicked();
	btnShowExhausted.toggle(true);
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

	const auto& mineFacility = *mSelectedFacility;
	btnIdle.toggle(mineFacility.isIdle());
	btnIdle.enabled(mineFacility.operational() || mineFacility.isIdle());

	btnDigNewLevel.toggle(mineFacility.extending());
	btnDigNewLevel.enabled(mineFacility.canExtend() && (mineFacility.operational() || mineFacility.isIdle()));
}


void MineReport::onIdle()
{
	mSelectedFacility->forceIdle(btnIdle.isPressed());
}


void MineReport::onDigNewLevel()
{
	auto& mineFacility = *mSelectedFacility;
	mineFacility.extend();

	btnDigNewLevel.toggle(mineFacility.extending());
	btnDigNewLevel.enabled(mineFacility.canExtend());
}


void MineReport::onTakeMeThere()
{
	if (mTakeMeThereHandler) { mTakeMeThereHandler(mSelectedFacility); }
}


void MineReport::onAddTruck()
{
	if (!mSelectedFacility) { return; }

	auto& mineFacility = *mSelectedFacility;

	if (mineFacility.assignedTrucks() == mineFacility.maxTruckCount()) { return; }

	if (pullTruckFromInventory())
	{
		mineFacility.addTruck();
		mAvailableTrucks = getTruckAvailability();
	}
}


void MineReport::onRemoveTruck()
{
	if (!mSelectedFacility) { return; }

	auto& mineFacility = *mSelectedFacility;

	if (mineFacility.assignedTrucks() == 1) { return; }

	if (pushTruckIntoInventory())
	{
		mineFacility.removeTruck();
		mAvailableTrucks = getTruckAvailability();
	}
}


void MineReport::drawMineFacilityPane(const NAS2D::Point<int>& origin) const
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(mineFacilityImage, origin);
	const auto text = mSelectedFacility ? getStructureDescription(*mSelectedFacility) : "";
	renderer.drawText(fontBigBold, text, origin + NAS2D::Vector{0, -33}, constants::PrimaryTextColor);

	drawStatusPane(origin + NAS2D::Vector{138, 0});
}


void MineReport::drawStatusPane(const NAS2D::Point<int>& origin) const
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawText(fontMediumBold, "Status", origin, constants::PrimaryTextColor);

	const auto& mineFacility = *mSelectedFacility;
	const bool isStatusHighlighted = mineFacility.disabled() || mineFacility.destroyed();
	const auto statusPosition = origin + NAS2D::Vector{0, fontMediumBold.height() + constants::MarginTight};
	renderer.drawText(fontMedium, mineFacility.stateDescription(), statusPosition, (isStatusHighlighted ? NAS2D::Color::Red : constants::PrimaryTextColor));

	if (mineFacility.destroyed() || mineFacility.underConstruction())
	{
		return;
	}

	const auto titleSpacing = NAS2D::Vector{0, fontMediumBold.height() + constants::MarginTight};
	const auto trucksOrigin = origin + NAS2D::Vector{0, fontMediumBold.height() + fontMedium.height() + constants::MarginTight * 2};
	renderer.drawText(fontMediumBold, "Trucks", trucksOrigin, constants::PrimaryTextColor);

	const auto truckValueOrigin = trucksOrigin + titleSpacing;
	const auto valueSpacing = NAS2D::Vector{0, font.height() + constants::MarginTight};
	const auto labelWidth = btnAddTruck.position().x - trucksOrigin.x - 10;
	drawLabelAndValueRightJustify(
		truckValueOrigin,
		labelWidth,
		"Assigned to Facility",
		std::to_string(mineFacility.assignedTrucks()),
		constants::PrimaryTextColor
	);
	drawLabelAndValueRightJustify(
		truckValueOrigin + valueSpacing,
		labelWidth,
		"Available in Storage",
		std::to_string(mAvailableTrucks),
		constants::PrimaryTextColor
	);

	if (!(mineFacility.operational() || mineFacility.isIdle()))
	{
		return;
	}

	const auto routeOrigin = truckValueOrigin + valueSpacing * 2;
	renderer.drawText(fontMediumBold, "Route", routeOrigin, constants::PrimaryTextColor);

	bool routeAvailable = hasRoute(mSelectedFacility);

	const auto routeValueOrigin = routeOrigin + titleSpacing;
	drawLabelAndValueRightJustify(
		routeValueOrigin,
		labelWidth,
		"Available",
		routeAvailable ? "Yes" : "No",
		routeAvailable ? constants::PrimaryTextColor : NAS2D::Color::Red
	);

	if (!routeAvailable)
	{
		return;
	}

	const auto routeCost = getRouteCost(mSelectedFacility);
	drawLabelAndValueRightJustify(
		routeValueOrigin + valueSpacing,
		labelWidth,
		"Cost",
		formatRouteCost(routeCost),
		constants::PrimaryTextColor
	);

	const auto oreMovementTotal = getOreHaulCapacity(mSelectedFacility);
	drawLabelAndValueRightJustify(
		routeValueOrigin + valueSpacing * 2,
		labelWidth,
		"Total Haul Capacity",
		std::to_string(oreMovementTotal),
		constants::PrimaryTextColor
	);
}


void MineReport::drawOreProductionPane(const NAS2D::Point<int>& origin) const
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawText(fontMediumBold, "Ore Production", origin, constants::PrimaryTextColor);
	const auto panelWidth = renderer.size().x - origin.x - 10;
	drawLabelRightJustify(origin, panelWidth, fontMediumBold, "Haul Capacity", constants::PrimaryTextColor);
	const auto lineOffset = NAS2D::Vector{0, fontMediumBold.height() + 1};
	const auto lineOrigin = origin + lineOffset;
	renderer.drawLine(lineOrigin, lineOrigin + NAS2D::Vector{panelWidth, 0}, constants::PrimaryTextColor, 1);

	const auto& oreDeposit = mSelectedFacility->oreDeposit();
	const auto oreAvailable = oreDeposit.availableResources();
	const auto oreTotalYield = oreDeposit.totalYield();

	const auto oreMovementTotal = getOreHaulCapacity(mSelectedFacility);
	const auto oreMovementComponent = oreMovementTotal / 4;
	const auto oreMovementRemainder = oreMovementComponent + (oreMovementTotal % 4);

	auto resourceOffset = lineOffset + NAS2D::Vector{0, 1 + constants::Margin + 2};
	const auto progressBarSize = NAS2D::Vector{renderer.size().x - origin.x - 10, std::max(25, fontBold.height() + constants::MarginTight * 2)};
	for (size_t i = 0; i < 4; ++i)
	{
		const auto resourcePosition = origin + resourceOffset;
		renderer.drawSubImage(uiIcons, resourcePosition, ResourceImageRectsOre[i]);
		const auto resourceNameOffset = NAS2D::Vector{ResourceImageRectsOre[i].size.x + constants::MarginTight + 2, 0};
		renderer.drawText(fontBold, "Mine " + ResourceNamesOre[i], resourcePosition + resourceNameOffset, constants::PrimaryTextColor);
		const auto oreMovement = (i != 3) ? oreMovementComponent : oreMovementRemainder;
		drawLabelRightJustify(resourcePosition, panelWidth, font, std::to_string(oreMovement), constants::PrimaryTextColor);

		const auto resourceNameHeight = std::max(ResourceImageRectsOre[i].size.y, fontBold.height());
		const auto progressBarPosition = resourcePosition + NAS2D::Vector{0, resourceNameHeight + constants::MarginTight + 2};
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


void MineReport::update()
{
	ControlContainer::update();
	draw();
}


void MineReport::draw() const
{
	auto& renderer = Utility<Renderer>::get();

	const auto startPoint = NAS2D::Point{area().center().x , area().position.y + 10};

	renderer.drawLine(startPoint, startPoint + NAS2D::Vector{0, area().size.y - 20}, constants::PrimaryTextColor);

	if (mSelectedFacility)
	{
		drawMineFacilityPane(startPoint + NAS2D::Vector{10, 30});
		drawOreProductionPane(startPoint + NAS2D::Vector{10, 260});
	}
}
