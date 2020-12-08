#include "MineReport.h"

#include "../TextRender.h"

#include "../../Cache.h"
#include "../../Constants.h"
#include "../../StructureManager.h"
#include "../../ProductionCost.h"

#include "../../States/Route.h"

#include "../../Things/Structures/MineFacility.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>
#include <cfloat>
#include <map>


using namespace NAS2D;


MineReport::MineReport() :
	font{ fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL) },
	fontBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL) },
	fontMedium{ fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM) },
	fontMediumBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM) },
	fontBigBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_HUGE) },
	mineFacility{ imageCache.load("ui/interface/mine.png") },
	uiIcons{ imageCache.load("ui/icons.png") },
	btnShowAll{ "All" },
	btnShowActive{ "Active" },
	btnShowIdle{ "Idle" },
	btnShowTappedOut{ "Tapped Out" },
	btnShowDisabled{ "Disabled" },
	btnIdle{ constants::BUTTON_IDLE },
	btnDigNewLevel{ "Dig New Level" },
	btnTakeMeThere{ constants::BUTTON_TAKE_ME_THERE },
	btnAddTruck { constants::BUTTON_ADD_TRUCK },
	btnRemoveTruck{ constants::BUTTON_REMOVE_TRUCK },
	chkCommonMetals{ "Mine Common Metals" },
	chkCommonMinerals{ "Mine Common Minerals" },
	chkRareMetals{ "Mine Rare Metals" },
	chkRareMinerals{ "Mine Rare Minerals" }
{

	add(btnShowAll, {10, 10});
	btnShowAll.size({ 75, 20 });
	btnShowAll.type(Button::Type::BUTTON_TOGGLE);
	btnShowAll.toggle(true);
	btnShowAll.click().connect(this, &MineReport::btnShowAllClicked);

	add(btnShowActive, {87, 10});
	btnShowActive.size({ 75, 20 });
	btnShowActive.type(Button::Type::BUTTON_TOGGLE);
	btnShowActive.click().connect(this, &MineReport::btnShowActiveClicked);

	add(btnShowIdle, {164, 10});
	btnShowIdle.size({ 75, 20 });
	btnShowIdle.type(Button::Type::BUTTON_TOGGLE);
	btnShowIdle.click().connect(this, &MineReport::btnShowIdleClicked);

	add(btnShowTappedOut, {241, 10});
	btnShowTappedOut.size({ 75, 20 });
	btnShowTappedOut.type(Button::Type::BUTTON_TOGGLE);
	btnShowTappedOut.click().connect(this, &MineReport::btnShowTappedOutClicked);

	add(btnShowDisabled, {318, 10});
	btnShowDisabled.size({ 75, 20 });
	btnShowDisabled.type(Button::Type::BUTTON_TOGGLE);
	btnShowDisabled.click().connect(this, &MineReport::btnShowDisabledClicked);

	add(lstMineFacilities, {10, 40});
	lstMineFacilities.selectionChanged().connect(this, &MineReport::lstMineFacilitySelectionChanged);

	// DETAIL PANE
	add(btnIdle, {0, 40});
	btnIdle.type(Button::Type::BUTTON_TOGGLE);
	btnIdle.size({ 140, 30 });
	btnIdle.click().connect(this, &MineReport::btnIdleClicked);

	add(btnDigNewLevel, {0, 75});
	btnDigNewLevel.size({ 140, 30 });
	btnDigNewLevel.click().connect(this, &MineReport::btnDigNewLevelClicked);

	add(btnTakeMeThere, {0, 110});
	btnTakeMeThere.size({ 140, 30 });
	btnTakeMeThere.click().connect(this, &MineReport::btnTakeMeThereClicked);

	// Ore Management Pane
	add(chkCommonMetals, {0, 210});
	chkCommonMetals.click().connect(this, &MineReport::chkCommonMetalsClicked);

	add(chkCommonMinerals, {0, 280});
	chkCommonMinerals.click().connect(this, &MineReport::chkCommonMineralsClicked);

	add(chkRareMetals, {0, 350});
	chkRareMetals.click().connect(this, &MineReport::chkRareMetalsClicked);

	add(chkRareMinerals, {0, 420});
	chkRareMinerals.click().connect(this, &MineReport::chkRareMineralsClicked);

	// Truck Management Pane
	add(btnAddTruck, {0, 215});
	btnAddTruck.size({ 140, 30 });
	btnAddTruck.click().connect(this, &MineReport::btnAddTruckClicked);

	add(btnRemoveTruck, {0, 250});
	btnRemoveTruck.size({ 140, 30 });
	btnRemoveTruck.click().connect(this, &MineReport::btnRemoveTruckClicked);

	Control::resized().connect(this, &MineReport::resized);
	fillLists();
}


void MineReport::selectStructure(Structure* structure)
{
	lstMineFacilities.currentSelection(structure);
}


void MineReport::clearSelection()
{
	lstMineFacilities.clearSelection();
	selectedFacility = nullptr;
}


void MineReport::refresh()
{
	btnShowAllClicked();
}


void MineReport::fillLists()
{
	lstMineFacilities.clearItems();
	std::size_t id = 1;
	for (auto facility : NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Mine))
	{
		lstMineFacilities.addItem(facility);

		/**
		 * Add a numeric ID to the text to avoid a monotonous look in the structure list.
		 * Since numeric structure ID's were done away with this is purely a cosmetic thing
		 * that could haunt us later if the player is looking to search for a mine by id
		 * as these are going to change between play/save/load sessions and most during
		 * gameplay as well since list position is not guaranteed.
		 */
		lstMineFacilities.last()->text = facility->name() + " #" + std::to_string(id);
		++id;
	}

	selectedFacility == nullptr ? lstMineFacilities.setSelection(0) : lstMineFacilities.currentSelection(selectedFacility);
	mAvailableTrucks = getTruckAvailability();
	updateManagementButtonsVisiblity();
}


void MineReport::resized(Control* /*c*/)
{
	lstMineFacilities.size({ rect().center().x - 20, rect().height - 51 });

	int position_x = rect().width - 150;
	btnIdle.position({ position_x, btnIdle.positionY() });
	btnDigNewLevel.position({ position_x, btnDigNewLevel.positionY() });
	btnTakeMeThere.position({ position_x, btnTakeMeThere.positionY() });

	auto& renderer = NAS2D::Utility<Renderer>::get();
	btnAddTruck.position({ position_x, renderer.size().y - 130 });
	btnRemoveTruck.position({ position_x, renderer.size().y - 95 });

	position_x -= 20;
	chkCommonMetals.position({ position_x, chkCommonMetals.positionY() });
	chkCommonMinerals.position({ position_x, chkCommonMinerals.positionY() });
	chkRareMetals.position({ position_x, chkRareMetals.positionY() });
	chkRareMinerals.position({ position_x, chkRareMinerals.positionY() });
}


void MineReport::filterButtonClicked()
{
	btnShowAll.toggle(false);
	btnShowActive.toggle(false);
	btnShowIdle.toggle(false);
	btnShowTappedOut.toggle(false);
	btnShowDisabled.toggle(false);
}


void MineReport::btnShowAllClicked()
{
	filterButtonClicked();
	btnShowAll.toggle(true);

	fillLists();
}


void MineReport::btnShowActiveClicked()
{
	filterButtonClicked();
	btnShowActive.toggle(true);
}


void MineReport::btnShowIdleClicked()
{
	filterButtonClicked();
	btnShowIdle.toggle(true);
}


void MineReport::btnShowTappedOutClicked()
{
	filterButtonClicked();
	btnShowTappedOut.toggle(true);
}


void MineReport::btnShowDisabledClicked()
{
	filterButtonClicked();
	btnShowDisabled.toggle(true);
}


void MineReport::btnIdleClicked()
{
	selectedFacility->forceIdle(btnIdle.toggled());
}


void MineReport::btnDigNewLevelClicked()
{
	auto facility = static_cast<MineFacility*>(selectedFacility);
	facility->extend();

	btnDigNewLevel.toggle(facility->extending());
	btnDigNewLevel.enabled(facility->canExtend());
}


void MineReport::btnTakeMeThereClicked()
{
	takeMeThereCallback()(selectedFacility);
}


void MineReport::btnAddTruckClicked()
{
	if (!selectedFacility) { return; }

	auto mFacility = static_cast<MineFacility*>(selectedFacility);

	if (mFacility->assignedTrucks() == mFacility->maxTruckCount()) { return; }

	if (pullTruckFromInventory())
	{
		mFacility->addTruck();
		mAvailableTrucks = getTruckAvailability();
	}
}


void MineReport::btnRemoveTruckClicked()
{
	auto mFacility = static_cast<MineFacility*>(selectedFacility);

	if (!selectedFacility) { return; }

	if (mFacility->assignedTrucks() == 1) { return; }

	if (pushTruckIntoInventory())
	{
		mFacility->removeTruck();
		mAvailableTrucks = getTruckAvailability();
	}
}


void MineReport::chkCommonMetalsClicked()
{
	MineFacility* facility = static_cast<MineFacility*>(selectedFacility);
	facility->mine()->miningCommonMetals(chkCommonMetals.checked());
}


void MineReport::chkCommonMineralsClicked()
{
	MineFacility* facility = static_cast<MineFacility*>(selectedFacility);
	facility->mine()->miningCommonMinerals(chkCommonMinerals.checked());
}


void MineReport::chkRareMetalsClicked()
{
	MineFacility* facility = static_cast<MineFacility*>(selectedFacility);
	facility->mine()->miningRareMetals(chkRareMetals.checked());
}


void MineReport::chkRareMineralsClicked()
{
	MineFacility* facility = static_cast<MineFacility*>(selectedFacility);
	facility->mine()->miningRareMinerals(chkRareMinerals.checked());
}


void MineReport::updateManagementButtonsVisiblity()
{
	btnIdle.visible(selectedFacility);
	btnDigNewLevel.visible(selectedFacility);
	btnTakeMeThere.visible(selectedFacility);

	btnAddTruck.visible(selectedFacility);
	btnRemoveTruck.visible(selectedFacility);

	chkCommonMetals.visible(selectedFacility);
	chkCommonMinerals.visible(selectedFacility);
	chkRareMetals.visible(selectedFacility);
	chkRareMinerals.visible(selectedFacility);
}


void MineReport::lstMineFacilitySelectionChanged()
{
	selectedFacility = lstMineFacilities.selectedStructure();

	updateManagementButtonsVisiblity();

	if (selectedFacility)
	{
		auto facility = static_cast<MineFacility*>(selectedFacility);
		btnIdle.toggle(facility->state() == StructureState::Idle);
		btnIdle.enabled(facility->state() == StructureState::Operational || facility->state() == StructureState::Idle);

		btnDigNewLevel.toggle(facility->extending());
		btnDigNewLevel.enabled(facility->canExtend());

		chkCommonMetals.checked(facility->mine()->miningCommonMetals());
		chkCommonMinerals.checked(facility->mine()->miningCommonMinerals());
		chkRareMetals.checked(facility->mine()->miningRareMetals());
		chkRareMinerals.checked(facility->mine()->miningRareMinerals());
	}
}


void MineReport::drawMineFacilityPane(const NAS2D::Point<int>& origin)
{
	auto& r = Utility<Renderer>::get();
	const auto textColor = NAS2D::Color{ 0, 185, 0 };

	r.drawImage(mineFacility, origin);
	r.drawText(fontBigBold, lstMineFacilities.selectionText(), origin + NAS2D::Vector{ 0, -33 }, textColor);

	r.drawText(fontMediumBold, "Status", origin + NAS2D::Vector{ 138, 0 }, textColor);

	bool isStatusHighlighted = selectedFacility->disabled() || selectedFacility->destroyed();
	auto statusPosition = btnIdle.position() - NAS2D::Vector{ fontMedium.width(selectedFacility->stateDescription()) + 5, 0 };
	r.drawText(fontMedium, selectedFacility->stateDescription(), statusPosition, (isStatusHighlighted ? NAS2D::Color::Red : textColor));

	auto resourceTextOrigin = origin + NAS2D::Vector{ 138, 30 };

	MineFacility* facility = static_cast<MineFacility*>(selectedFacility);
	const auto& mine = *facility->mine();

	const auto barOrigin = resourceTextOrigin.x + 125;
	const auto barWidth = btnIdle.positionX() - barOrigin - 10;

	for (size_t i = 0; i < ResourceNamesOre.size(); ++i)
	{
		r.drawText(font, ResourceNamesOre[i], resourceTextOrigin, textColor);

		const float percent = static_cast<float>(mine.oreAvailable(i)) / static_cast<float>(mine.oreTotalYield(i));

		drawBasicProgressBar(barOrigin, resourceTextOrigin.y, barWidth, 12, percent, 2);
		resourceTextOrigin.y += 20;
	}
}


void MineReport::drawOreProductionPane(const NAS2D::Point<int>& origin)
{
	auto& renderer = Utility<Renderer>::get();
	const auto textColor = NAS2D::Color{ 0, 185, 0 };
	const auto& mine = *static_cast<MineFacility*>(selectedFacility)->mine();

	renderer.drawText(fontMediumBold, "Ore Production", origin, textColor);
	renderer.drawLine(origin + NAS2D::Vector{ 0, 21 }, { static_cast<float>(renderer.size().x - 10), static_cast<float>(origin.y + 21) }, textColor, 1);

	int offsetY = 0;
	const int barWidth = renderer.size().x - origin.x - 10;
	for (size_t i = 0; i < 4; ++i)
	{
		renderer.drawSubImage(uiIcons, origin + NAS2D::Vector{ 0, 30 + offsetY }, NAS2D::Rectangle{ 64, 0, 16, 16 });
		renderer.drawText(fontBold, ResourceNamesOre[0], origin + NAS2D::Vector{ 20, 30 + offsetY }, textColor);

		const auto percent = static_cast<float>(mine.oreAvailable(i)) / static_cast<float>(mine.oreTotalYield(i));
		drawBasicProgressBar(origin.x, origin.y + 50 + offsetY, barWidth, 25, percent);

		const std::string str = std::to_string(mine.oreAvailable(i)) + " of " + std::to_string(mine.oreTotalYield(i)) + " Remaining";
		const int strOffsetX = (barWidth / 2) - (fontBold.width(str) / 2);
		const int strOffsetY = (fontBold.height() / 2) - 1;
		renderer.drawText(fontBold, str, origin + NAS2D::Vector{ strOffsetX, 50 + offsetY + strOffsetY });

		offsetY += 70;
	}
}


void MineReport::drawTruckMangementPane(const NAS2D::Point<int>& origin)
{
	auto& r = Utility<Renderer>::get();
	const auto textColor = NAS2D::Color{ 0, 185, 0 };
	const auto mFacility = static_cast<MineFacility*>(selectedFacility);

	r.drawText(fontMediumBold, "Trucks & Routing", origin, textColor);
	r.drawLine(origin + NAS2D::Vector{ 0, 21 }, { static_cast<float>(r.size().x - 10), static_cast<float>(origin.y + 21) }, textColor, 1);

	r.drawText(fontBold, "Trucks Assigned to Facility", origin + NAS2D::Vector{ 0, 30 }, textColor);

	const auto labelWidth = btnAddTruck.positionX() - origin.x - 10;
	drawLabelAndValueRightJustify(origin + NAS2D::Vector{ 0, 30 }, labelWidth, "Trucks Assigned to Facility", std::to_string(mFacility->assignedTrucks()), textColor);
	drawLabelAndValueRightJustify(origin + NAS2D::Vector{ 0, 45 }, labelWidth, "Trucks Available in Storage", std::to_string(mAvailableTrucks), textColor);

	auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	bool routeAvailable = routeTable.find(mFacility) != routeTable.end();

	drawLabelAndValueRightJustify(origin + NAS2D::Vector{ 0, 65 },
		labelWidth,
		"Route Available",
		routeAvailable ? "Yes" : "No",
		routeAvailable ? textColor : NAS2D::Color::Red);

	if (routeAvailable)
	{
		drawTruckHaulInfo(origin + NAS2D::Vector{ 0, 80 });
	}
}


void MineReport::drawTruckHaulInfo(const NAS2D::Point<int>& origin)
{
	auto& r = Utility<Renderer>::get();
	const auto textColor = NAS2D::Color{ 0, 185, 0 };
	auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	const auto mFacility = static_cast<MineFacility*>(selectedFacility);

	auto& route = routeTable[mFacility];
	drawLabelAndValueRightJustify(origin,
		btnAddTruck.positionX() - origin.x - 10,
		"Route Cost",
		std::to_string(route.cost).substr(0, std::to_string(route.cost).find(".") + 3), // hack-ish and probably slow, this could be cached
		textColor);


	const float routeCost = std::clamp(route.cost, 1.0f, FLT_MAX);
	const int totalOreMovement = static_cast<int>(constants::ShortestPathTraversalCount / routeCost) * mFacility->assignedTrucks();
	const int oreMovementLabelWidth = r.size().x - origin.x - 10;
	const int oreMovementPart = totalOreMovement / 4;
	const int oreLabelWidth = (oreMovementLabelWidth - 10) / 2;

	const NAS2D::Rectangle<int> tableRect({ origin.x - 2, origin.y + 18, oreMovementLabelWidth + 5, 47 });

	r.drawBoxFilled(tableRect, { 0, 0, 0, 100 });
	r.drawBox(tableRect, textColor);

	r.drawLine(origin + NAS2D::Vector{ 0, 34 }, origin + NAS2D::Vector{ oreMovementLabelWidth, 34 }, textColor);
	r.drawLine(origin + NAS2D::Vector{ 0, 50 }, origin + NAS2D::Vector{ oreMovementLabelWidth, 50 }, textColor);
	r.drawLine(origin + NAS2D::Vector{ oreLabelWidth + 5, 37 }, origin + NAS2D::Vector{ oreLabelWidth + 5, 63 }, textColor);

	drawLabelAndValueRightJustify(origin + NAS2D::Vector{ 0, 20 },
		oreMovementLabelWidth,
		"Total Haul Capacity per Turn",
		std::to_string(totalOreMovement),
		textColor);

	drawLabelAndValueRightJustify(origin + NAS2D::Vector{ 0, 35 },
		oreLabelWidth,
		ResourceNamesOre[0] + " Haul Capacity",
		std::to_string(oreMovementPart),
		textColor);

	drawLabelAndValueRightJustify(origin + NAS2D::Vector{ oreLabelWidth + 10, 35 },
		oreLabelWidth,
		ResourceNamesOre[1] + " Haul Capacity",
		std::to_string(oreMovementPart),
		textColor);

	drawLabelAndValueRightJustify(origin + NAS2D::Vector{ 0, 50 },
		oreLabelWidth,
		ResourceNamesOre[2] + " Haul Capacity",
		std::to_string(oreMovementPart),
		textColor);

	drawLabelAndValueRightJustify(origin + NAS2D::Vector{ oreLabelWidth + 10, 50 },
		oreLabelWidth,
		ResourceNamesOre[3] + " Haul Capacity",
		std::to_string(oreMovementPart + (totalOreMovement % 4)),
		textColor);
}


void MineReport::update()
{
	if (!visible()) { return; }

	auto& r = Utility<Renderer>::get();

	const auto textColor = NAS2D::Color{ 0, 185, 0 };
	const auto startPoint = NAS2D::Point{ rect().center().x , rect().y + 10 };

	r.drawLine(startPoint, startPoint + NAS2D::Vector{ 0, rect().height - 20 }, textColor);

	if (selectedFacility)
	{
		drawMineFacilityPane(startPoint + NAS2D::Vector{ 10, 30 });
		drawOreProductionPane(startPoint + NAS2D::Vector{ 10, 170 });
		drawTruckMangementPane(startPoint + NAS2D::Vector{ 10, r.size().y - 214 });
	}

	UIContainer::update();
}
