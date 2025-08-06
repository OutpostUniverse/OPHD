// ==================================================================================
// = This file implements the UI and UI Event Handlers used by MapViewState. I separated
// = it into its own file because the MapViewState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "MapViewState.h"
#include "ColonyShip.h"
#include "MapViewStateHelper.h"

#include "MainMenuState.h"

#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include "../StructureCatalog.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"
#include "../Map/MapView.h"
#include "../MapObjects/Robots.h"
#include "../MapObjects/Structures/CommandCenter.h"
#include "../MapObjects/Structures/FoodProduction.h"

#include "../UI/MessageBox.h"
#include "../UI/NavControl.h"
#include "../UI/MiniMap.h"
#include "../UI/CheatMenu.h"

#include <libOPHD/DirectionOffset.h>
#include <libOPHD/EnumDirection.h>
#include <libOPHD/EnumStructureID.h>

#include <libOPHD/Map/MapCoordinate.h>
#include <libOPHD/MapObjects/StructureType.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


extern NAS2D::Point<int> MOUSE_COORDS;


/**
 * Sets up the user interface elements
 */
void MapViewState::initUi()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	mCheatMenu.hide();
	mDiggerDirection.hide();

	mTileInspector.position(renderer.center() - NAS2D::Vector{mTileInspector.size().x / 2.0f, 175.0f});
	mTileInspector.hide();

	mStructureInspector.position(renderer.center() - NAS2D::Vector{mStructureInspector.size().x / 2.0f, 175.0f});
	mStructureInspector.hide();

	mRobotInspector.position(renderer.center() - NAS2D::Vector{mRobotInspector.size().x / 2.0f, 175.0f});
	mRobotInspector.hide();

	mFactoryProduction.position(NAS2D::Point{renderer.center().x - mFactoryProduction.size().x / 2.0f, 175.0f});
	mFactoryProduction.hide();

	mFileIoDialog.anchored(true);
	mFileIoDialog.hide();

	mResourceBreakdownPanel.position({0, 22});
	mResourceBreakdownPanel.playerResources(&mResourcesCount);

	mGameOverDialog.hide();

	mGameOptionsDialog.hide();

	mAnnouncement.hide();
	mMineOperationsWindow.hide();
	mWarehouseInspector.hide();

	mWindowStack.addWindow(mTileInspector);
	mWindowStack.addWindow(mStructureInspector);
	mWindowStack.addWindow(mFactoryProduction);
	mWindowStack.addWindow(mDiggerDirection);
	mWindowStack.addWindow(mAnnouncement);
	mWindowStack.addWindow(mWarehouseInspector);
	mWindowStack.addWindow(mMineOperationsWindow);
	mWindowStack.addWindow(mRobotInspector);
	mWindowStack.addWindow(mNotificationWindow);
	mWindowStack.addWindow(mCheatMenu);

	mNotificationWindow.hide();

	// BUTTONS

	mBtnTurns.size(constants::MainButtonSize);
	mBtnTurns.enabled(false);

	mBtnToggleHeightmap.size(constants::MainButtonSize);
	mBtnToggleHeightmap.type(Button::Type::Toggle);

	mBtnToggleRouteOverlay.size(constants::MainButtonSize);
	mBtnToggleRouteOverlay.type(Button::Type::Toggle);

	mBtnToggleConnectedness.size(constants::MainButtonSize);
	mBtnToggleConnectedness.type(Button::Type::Toggle);

	mBtnToggleCommRangeOverlay.size(constants::MainButtonSize);
	mBtnToggleCommRangeOverlay.type(Button::Type::Toggle);

	mBtnTogglePoliceOverlay.size(constants::MainButtonSize);
	mBtnTogglePoliceOverlay.type(Button::Type::Toggle);

	// Initial Structures
	mMapObjectPicker.addStructure(StructureID::SeedLander);

	// tooltip control sizes
	constexpr auto hudHeight = constants::ResourceIconSize + constants::MarginTight * 2;

	mTooltipCurrentTurns.size({45, hudHeight});
	mTooltipSystemButton.size({hudHeight, hudHeight});

	// Tool Tips
	mToolTip.add(mBtnTurns, constants::ToolTipBtnTurns);
	mToolTip.add(mBtnToggleHeightmap, constants::ToolTipBtnHeightmap);
	mToolTip.add(mBtnToggleRouteOverlay, constants::ToolTipBtnRoutes);
	mToolTip.add(mBtnToggleConnectedness, constants::ToolTipBtnConnectedness);
	mToolTip.add(mBtnToggleCommRangeOverlay, constants::ToolTipBtnCommRange);
	mToolTip.add(mBtnTogglePoliceOverlay, constants::ToolTipBtnPolice);
	mToolTip.add(mTooltipCurrentTurns, constants::ToolTipCurrentTurns);
	mToolTip.add(mTooltipSystemButton, constants::ToolTipSystemMenu);
}


void MapViewState::setupUiPositions(NAS2D::Vector<int> size)
{
	mResourceInfoBar.width(size.x);

	const auto populationPanelX = std::min(675, size.x - mPopulationPanel.size().x);
	mPopulationPanel.position({populationPanelX, constants::ResourceIconSize + 4 + constants::MarginTight});

	// Bottom UI Area
	mBottomUiRect = {{0, size.y - constants::BottomUiHeight}, {size.x, constants::BottomUiHeight}};

	// Menu / System Icon
	mTooltipSystemButton.position({size.x - (constants::ResourceIconSize + constants::MarginTight * 2), 0});
	mTooltipCurrentTurns.position({size.x - 80 , 0});

	mRobotDeploymentSummary.area({{8, size.y - constants::BottomUiHeight - 8 - 100}, {200, 100}});

	// Mini Map
	mMiniMap->area({{size.x - 300 - constants::Margin, mBottomUiRect.position.y + constants::Margin}, {300, 150}});

	const auto navControlEndPoint = NAS2D::Point{size.x, mBottomUiRect.position.y};
	mNavControl->position(navControlEndPoint - mNavControl->size());

	// Notification Area
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	mNotificationArea.height(mNavControl->position().y - 22 - constants::Margin);
	mNotificationArea.position({renderer.size().x - mNotificationArea.size().x, 22});

	// Position UI Buttons
	const auto buttonColumnOrigin = mMiniMap->position() + NAS2D::Vector{-constants::MainButtonSize - constants::MarginTight, 0};
	mBtnTurns.position(NAS2D::Point{buttonColumnOrigin.x - constants::MainButtonSize, buttonColumnOrigin.y});
	mBtnToggleHeightmap.position({buttonColumnOrigin.x, buttonColumnOrigin.y});
	mBtnToggleRouteOverlay.position({buttonColumnOrigin.x, buttonColumnOrigin.y + constants::MainButtonSize});
	mBtnToggleConnectedness.position({buttonColumnOrigin.x, buttonColumnOrigin.y + constants::MainButtonSize * 2});
	mBtnToggleCommRangeOverlay.position({buttonColumnOrigin.x, buttonColumnOrigin.y + constants::MainButtonSize * 3});
	mBtnTogglePoliceOverlay.position({buttonColumnOrigin.x, buttonColumnOrigin.y + constants::MainButtonSize * 4});

	// UI Panels
	mMapObjectPicker.area({
		{constants::Margin, mBottomUiRect.position.y + constants::Margin},
		{mBtnTurns.position().x - constants::Margin - constants::MarginTight, constants::BottomUiHeight - constants::Margin * 2}
	});

	// Allow for centering with rounding to integer values
	const auto rendererCenter = NAS2D::Utility<NAS2D::Renderer>::get().center().to<int>();
	const auto centerPosition = [&rendererCenter](const Control& control) { return (rendererCenter - control.size() / 2); };

	// Anchored window positions
	mFileIoDialog.position(NAS2D::Point{centerPosition(mFileIoDialog).x, 50});
	mCheatMenu.position(NAS2D::Point{centerPosition(mCheatMenu).x, centerPosition(mCheatMenu).y});
	mGameOverDialog.position(centerPosition(mGameOverDialog) - NAS2D::Vector{0, 100});
	mAnnouncement.position(centerPosition(mAnnouncement) - NAS2D::Vector{0, 100});
	mGameOptionsDialog.position(centerPosition(mGameOptionsDialog) - NAS2D::Vector{0, 100});

	mDiggerDirection.position(NAS2D::Point{centerPosition(mDiggerDirection).x, size.y / 2 - 125});

	mWarehouseInspector.position(centerPosition(mWarehouseInspector) - NAS2D::Vector{0, 100});
	mMineOperationsWindow.position(centerPosition(mMineOperationsWindow) - NAS2D::Vector{0, 100});

	mNotificationWindow.position(centerPosition(mMineOperationsWindow) - NAS2D::Vector{0, 100});

	/**
	 * \note	We are not setting the tile inspector window's position here because it's something that can be
	 *			moved around by the user. Re-centering it here could annoy the user.
	 */
}


/**
 * Hides ALL UI elements.
 */
void MapViewState::hideUi()
{
	mBtnTurns.hide();

	mBtnToggleHeightmap.hide();
	mBtnToggleRouteOverlay.hide();
	mBtnToggleConnectedness.hide();
	mBtnToggleCommRangeOverlay.hide();
	mBtnTogglePoliceOverlay.hide();

	mMapObjectPicker.hide();

	mWindowStack.hide();
}


/**
 * Unhides the UI.
 */
void MapViewState::unhideUi()
{
	mBtnTurns.show();

	mBtnToggleHeightmap.show();
	mBtnToggleRouteOverlay.show();
	mBtnToggleConnectedness.show();
	mBtnToggleCommRangeOverlay.show();
	mBtnTogglePoliceOverlay.visible(true);

	mMapObjectPicker.show();

	mGameOverDialog.enabled(true);
	mGameOptionsDialog.enabled(true);
}


/**
 * Hides all non-essential UI elements.
 */
void MapViewState::resetUi()
{
	mMapObjectPicker.clearBuildMode();
	mMapObjectPicker.clearSelections();

	mWindowStack.hide();
}


/**
 * Adds selection options to the Structure Menu
 */
void MapViewState::populateStructureMenu()
{
	mStructures.clear();
	mConnections.clear();

	// Above Ground structures only
	if (mStructureManager.count() == 0)
	{
		if (mMapView->isSurface())
		{
			mMapObjectPicker.setStructureIds({StructureID::SeedLander});
		}
	}
	else if (mMapView->isSurface())
	{
		mMapObjectPicker.setStructureIds(mStructureTracker.availableSurfaceStructures());
		mMapObjectPicker.setTubesAboveGround();

		// Special case code, not thrilled with this
		if (mColonyShip.colonistLanders() > 0) { mMapObjectPicker.addStructure(StructureID::ColonistLander); }
		if (mColonyShip.cargoLanders() > 0) { mMapObjectPicker.addStructure(StructureID::CargoLander); }
	}
	else
	{
		mMapObjectPicker.setStructureIds(mStructureTracker.availableUndergroundStructures());
		mMapObjectPicker.setTubesUnderGround();
	}

	updateStructuresAvailability();

	mStructures.sort();
}


/**
 * Update IconGridItems availability
 */
void MapViewState::updateStructuresAvailability()
{
	for (std::size_t id = 1; id < StructureCatalog::count(); ++id)
	{
		const auto& structureType = StructureCatalog::getType(id);
		const auto hasSufficientResources = structureType.buildCost <= mResourcesCount;
		mStructures.itemAvailable(structureType.name, hasSufficientResources);
	}
}


/**
 * Some UI elements should be treated as 'modal' and block input when they're
 * visible. This is a central way to check for a modal UI element without
 * having to check for multiple elements in a variety of places in the code.
 */
bool MapViewState::modalUiElementDisplayed() const
{
	return mGameOptionsDialog.visible() ||
		mFileIoDialog.visible() ||
		mGameOverDialog.visible();
}


/**
* Updates and draws the UI.
*/
void MapViewState::drawUI()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// Bottom UI
	renderer.drawBoxFilled(mBottomUiRect, NAS2D::Color{39, 39, 39});
	renderer.drawBox(mBottomUiRect, NAS2D::Color{21, 21, 21});
	renderer.drawLine(NAS2D::Point{mBottomUiRect.position.x + 1, mBottomUiRect.position.y}, NAS2D::Point{mBottomUiRect.position.x + mBottomUiRect.size.x - 2, mBottomUiRect.position.y}, NAS2D::Color{56, 56, 56});

	mMiniMap->draw(renderer);
	mNavControl->draw(renderer);
	mRobotDeploymentSummary.draw(renderer);

	if (mResourceInfoBar.isPopulationPanelVisible()) { mPopulationPanel.update(); }
	if (mResourceInfoBar.isResourcePanelVisible()) { mResourceBreakdownPanel.update(); }

	mResourceInfoBar.update();
	drawSystemButton();

	mNotificationArea.update();

	// Buttons
	mBtnTurns.update();
	mBtnToggleHeightmap.update();
	mBtnToggleRouteOverlay.update();
	mBtnToggleConnectedness.update();
	mBtnToggleCommRangeOverlay.update();
	mBtnTogglePoliceOverlay.update();

	// Menus
	mMapObjectPicker.update();

	// Windows
	mFileIoDialog.update();
	mGameOptionsDialog.update();
	mWindowStack.update();

	if (!modalUiElementDisplayed()) { mToolTip.update(); }
}


void MapViewState::setOverlay(std::vector<Tile*>& tileList, Tile::Overlay overlay)
{
	for (auto tile : tileList)
	{
		tile->overlay(overlay);
	}
}


void MapViewState::clearOverlays()
{
	clearOverlay(mTruckRouteOverlay);
	clearOverlay(mConnectednessOverlay);
	clearOverlay(mCommRangeOverlay);
	clearOverlay(mPoliceOverlays[static_cast<std::size_t>(mMapView->currentDepth())]);
}


void MapViewState::clearOverlay(std::vector<Tile*>& tileList)
{
	setOverlay(tileList, Tile::Overlay::None);
}


void MapViewState::changePoliceOverlayDepth(int oldDepth, int newDepth)
{
	clearOverlay(mPoliceOverlays[static_cast<std::size_t>(oldDepth)]);
	setOverlay(mPoliceOverlays[static_cast<std::size_t>(newDepth)], Tile::Overlay::Police);
}


void MapViewState::onToggleHeightmap()
{
	mMiniMap->heightMapVisible(mBtnToggleHeightmap.isPressed());
}


/**
 * Handles clicks of the Connectedness Overlay button.
 */
void MapViewState::onToggleConnectedness()
{
	clearOverlays();

	if (mBtnToggleConnectedness.isPressed())
	{
		mBtnToggleRouteOverlay.toggle(false);
		mBtnToggleCommRangeOverlay.toggle(false);
		mBtnTogglePoliceOverlay.toggle(false);

		setOverlay(mConnectednessOverlay, Tile::Overlay::Connectedness);
	}
}


void MapViewState::onToggleCommRangeOverlay()
{
	clearOverlays();

	if (mBtnToggleCommRangeOverlay.isPressed())
	{
		mBtnToggleRouteOverlay.toggle(false);
		mBtnToggleConnectedness.toggle(false);
		mBtnTogglePoliceOverlay.toggle(false);

		setOverlay(mCommRangeOverlay, Tile::Overlay::Communications);
	}
}

void MapViewState::onTogglePoliceOverlay()
{
	clearOverlays();

	if (mBtnTogglePoliceOverlay.isPressed())
	{
		mBtnToggleRouteOverlay.toggle(false);
		mBtnToggleConnectedness.toggle(false);
		mBtnToggleCommRangeOverlay.toggle(false);

		setOverlay(mPoliceOverlays[static_cast<std::size_t>(mMapView->currentDepth())], Tile::Overlay::Police);
	}
}

void MapViewState::onToggleRouteOverlay()
{
	clearOverlays();

	if (mBtnToggleRouteOverlay.isPressed())
	{
		mBtnToggleConnectedness.toggle(false);
		mBtnToggleCommRangeOverlay.toggle(false);
		mBtnTogglePoliceOverlay.toggle(false);

		setOverlay(mTruckRouteOverlay, Tile::Overlay::TruckingRoutes);
	}
}


void MapViewState::onNotificationClicked(const NotificationArea::Notification& notification)
{
	mNotificationWindow.notification(notification);
	mNotificationWindow.show();
	mWindowStack.bringToFront(mNotificationWindow);
}


void MapViewState::onDiggerSelectionDialog(Direction direction, Tile& tile)
{
	// Before doing anything, if we're going down and the depth is not the surface,
	// the assumption is that we've already checked and determined that there's an air shaft
	// so clear it from the tile, disconnect the tile and run a connectedness search.
	if (tile.depth() > 0 && direction == Direction::Down)
	{
		mStructureManager.removeStructure(*tile.structure());
		updateConnectedness();
	}

	// Assumes a digger is available.
	mRobotPool.deployDigger(tile, direction);

	if (direction != Direction::Down)
	{
		mTileMap->getTile(tile.xyz().translate(direction)).excavate();
	}

	if (!mRobotPool.robotAvailable(RobotTypeIndex::Digger))
	{
		mRobots.removeItem(constants::Robodigger);
		mMapObjectPicker.clearBuildMode();
	}

	mDiggerDirection.visible(false);
}


/**
 * Click handler for the main menu Save Game button.
 */
void MapViewState::onOpenSaveGameDialog()
{
	mGameOptionsDialog.hide();
	mFileIoDialog.showSave(constants::SaveGamePath);
}


/**
 * Click handler for the main menu Load Game button.
 */
void MapViewState::onOpenLoadGameDialog()
{
	mGameOptionsDialog.hide();
	mFileIoDialog.showOpen(constants::SaveGamePath);
}


/**
 * Click handler for the main menu Return to Game button.
 */
void MapViewState::onReturnToGame()
{
	mGameOptionsDialog.hide();
}


/**
 * Click handler for the main menu Return to Main Menu Screen button.
 */
void MapViewState::onGameOver()
{
	mFade.fadeOut(constants::FadeSpeed);
	if (mQuitHandler) { mQuitHandler(); }
}


void MapViewState::onTakeMeThere(const MapCoordinate& position)
{
	mMapView->centerOn(position);
	populateStructureMenu();
}


/**
 * Turns button clicked.
 */
void MapViewState::onTurns()
{
	nextTurn();
}

void MapViewState::onCheatCodeEntry(CheatMenu::CheatCode cheatCode)
{
	switch(cheatCode)
	{
		case CheatMenu::CheatCode::Invalid:
			return;
		case CheatMenu::CheatCode::AddResources:
			addRefinedResources({1000, 1000, 1000, 1000});
		break;
		case CheatMenu::CheatCode::AddFood:
		{
			auto foodProducers = mStructureManager.getStructures<FoodProduction>();
			const auto& command = mStructureManager.getStructures<CommandCenter>();
			foodProducers.insert(foodProducers.begin(), command.begin(), command.end());

			for (auto* foodProducer : foodProducers)
			{
				foodProducer->foodLevel(foodProducer->foodStorageCapacity());
			}
		}
		break;
		case CheatMenu::CheatCode::AddChildren:
			mPopulation.addPopulation({10, 0, 0, 0, 0});
		break;
		case CheatMenu::CheatCode::AddStudents:
			mPopulation.addPopulation({0, 10, 0, 0, 0});
		break;
		case CheatMenu::CheatCode::AddWorkers:
			mPopulation.addPopulation({0, 0, 10, 0, 0});
		break;
		case CheatMenu::CheatCode::AddScientists:
			mPopulation.addPopulation({0, 0, 0, 10, 0});
		break;
		case CheatMenu::CheatCode::AddRetired:
			mPopulation.addPopulation({0, 0, 0, 0, 10});
		break;
		case CheatMenu::CheatCode::RemoveChildren:
			mPopulation.removePopulation({10, 0, 0, 0, 0});
		break;
		case CheatMenu::CheatCode::RemoveStudents:
			mPopulation.removePopulation({0, 10, 0, 0, 0});
		break;
		case CheatMenu::CheatCode::RemoveWorkers:
			mPopulation.removePopulation({0, 0, 10, 0, 0});
		break;
		case CheatMenu::CheatCode::RemoveScientists:
			mPopulation.removePopulation({0, 0, 0, 10, 0});
		break;
		case CheatMenu::CheatCode::RemoveRetired:
			mPopulation.removePopulation({0, 0, 0, 0, 10});
		break;
		case CheatMenu::CheatCode::AddRobots:
			mRobotPool.addRobot(RobotTypeIndex::Digger);
			mRobotPool.addRobot(RobotTypeIndex::Dozer);
			mRobotPool.addRobot(RobotTypeIndex::Miner);
		break;

	}
	updatePlayerResources();
	updateStructuresAvailability();
	updateFood();
}
