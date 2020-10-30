#include "MineReport.h"

#include "../TextRender.h"
#include "../../Cache.h"
#include "../../Constants.h"
#include "../../StructureManager.h"
#include "../../ProductionCost.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <map>
#include <array>


using namespace NAS2D;


MineReport::MineReport() :
	font{ fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL) },
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
	btnIdle{ "Idle" },
	btnDigNewLevel{ "Dig New Level" },
	btnTakeMeThere{ constants::BUTTON_TAKE_ME_THERE }
{

	add(&btnShowAll, 10, 10);
	btnShowAll.size({ 75, 20 });
	btnShowAll.type(Button::Type::BUTTON_TOGGLE);
	btnShowAll.toggle(true);
	btnShowAll.click().connect(this, &MineReport::btnShowAllClicked);

	add(&btnShowActive, 87, 10);
	btnShowActive.size({ 75, 20 });
	btnShowActive.type(Button::Type::BUTTON_TOGGLE);
	btnShowActive.click().connect(this, &MineReport::btnShowActiveClicked);

	add(&btnShowIdle, 164, 10);
	btnShowIdle.size({ 75, 20 });
	btnShowIdle.type(Button::Type::BUTTON_TOGGLE);
	btnShowIdle.click().connect(this, &MineReport::btnShowIdleClicked);

	add(&btnShowTappedOut, 241, 10);
	btnShowTappedOut.size({ 75, 20 });
	btnShowTappedOut.type(Button::Type::BUTTON_TOGGLE);
	btnShowTappedOut.click().connect(this, &MineReport::btnShowTappedOutClicked);

	add(&btnShowDisabled, 318, 10);
	btnShowDisabled.size({ 75, 20 });
	btnShowDisabled.type(Button::Type::BUTTON_TOGGLE);
	btnShowDisabled.click().connect(this, &MineReport::btnShowDisabledClicked);

	add(&lstMineFacilities, 10, 40);
	lstMineFacilities.selectionChanged().connect(this, &MineReport::lstMineFacilitySelectionChanged);

	// DETAIL PANE
	add(&btnIdle, 0, 40);
	btnIdle.type(Button::Type::BUTTON_TOGGLE);
	btnIdle.size({ 140, 30 });
	btnIdle.click().connect(this, &MineReport::btnIdleClicked);

	add(&btnDigNewLevel, 0, 80);
	btnDigNewLevel.size({ 140, 30 });
	btnDigNewLevel.click().connect(this, &MineReport::btnDigNewLevelClicked);

	add(&btnTakeMeThere, 0, 120);
	btnTakeMeThere.size({ 140, 30 });
	btnTakeMeThere.click().connect(this, &MineReport::btnTakeMeThereClicked);

	Control::resized().connect(this, &MineReport::resized);
	fillLists();
}


/**
 * Override of the interface provided by ReportInterface class.
 *
 * \note	Pointer
 */
void MineReport::selectStructure(Structure* structure)
{
	lstMineFacilities.currentSelection(structure);
}


void MineReport::clearSelection()
{
	lstMineFacilities.clearSelection();
	selectedFacility = nullptr;
}


/**
 * Pass-through function to simulate clicking on the Show All button.
 */
void MineReport::refresh()
{
	btnShowAllClicked();
}


/**
 * Fills the factory list with all available factories.
 */
void MineReport::fillLists()
{
	lstMineFacilities.clearItems();
	std::size_t id = 1;
	for (auto facility : NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Mine))
	{
		lstMineFacilities.addItem(facility);
		
		/**
		 * Adding a numeric ID to the text to avoid a monotonous look in the structure list.
		 * Since numeric structure ID's were done away with this is purely a cosmetic thing
		 * that could haunt us later if the player is looking to search for a mine by id
		 * as these are going to change between play/save/load sessions and most during
		 * gameplay as well since list position is not guaranteed.
		 */
		lstMineFacilities.last()->text = facility->name() + " #" + std::to_string(id);
		++id;
	}

	lstMineFacilities.currentSelection(selectedFacility);
}


void MineReport::resized(Control* /*c*/)
{
	lstMineFacilities.size({ rect().center().x - 20, rect().height - 50 });

	int position_x = rect().width - 150;
	btnIdle.position({ position_x, btnIdle.positionY() });
	btnDigNewLevel.position({ position_x, btnDigNewLevel.positionY() });
	btnTakeMeThere.position({ position_x, btnTakeMeThere.positionY() });
}


/**
 * This has been overridden to handle some UI elements that need
 * special handling.
 */
void MineReport::visibilityChanged(bool visible)
{
	if (!visible) { return; }
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

}


void MineReport::btnDigNewLevelClicked()
{

}


void MineReport::btnTakeMeThereClicked()
{

}


void MineReport::lstMineFacilitySelectionChanged()
{
	selectedFacility = lstMineFacilities.selectedStructure();
}


void MineReport::drawMineFacilityPane(const NAS2D::Point<int>& startPoint)
{
	if (!selectedFacility) { return; }

	auto& r = Utility<Renderer>::get();
	const auto textColor = NAS2D::Color{ 0, 185, 0 };

	const auto origin = startPoint + NAS2D::Vector{ 10, 30 };

	r.drawImage(mineFacility, origin);
	r.drawText(fontBigBold, lstMineFacilities.selectionText(), origin + NAS2D::Vector{ 0, -33 }, textColor);
	
	r.drawText(fontMediumBold, "Status", origin + NAS2D::Vector{ 138, 0 }, textColor);

	bool isStatusHighlighted = selectedFacility->disabled() || selectedFacility->destroyed();
	const auto statusPosition = btnIdle.position() - NAS2D::Vector{ fontMedium.width(selectedFacility->stateDescription()) + 10, 0 };
	r.drawText(fontMedium, selectedFacility->stateDescription(), statusPosition, (isStatusHighlighted ? NAS2D::Color::Red : textColor));
}


void MineReport::update()
{
	if (!visible()) { return; }

	auto& renderer = Utility<Renderer>::get();

	const auto textColor = NAS2D::Color{ 0, 185, 0 };
	const auto startPoint = NAS2D::Point{ rect().center().x , rect().y + 10 };

	renderer.drawLine(startPoint, startPoint + NAS2D::Vector{ 0, rect().height - 20 }, textColor);

	drawMineFacilityPane(startPoint);
	
	UIContainer::update();
}
