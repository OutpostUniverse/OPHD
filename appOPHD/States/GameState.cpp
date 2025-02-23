#include "GameState.h"

#include "MainMenuState.h"
#include "MapViewState.h"
#include "MainReportsUiState.h"
#include "Wrapper.h"
#include "../StructureManager.h"
#include "../UI/MessageBox.h"
#include "../Constants/Strings.h"

#include <NAS2D/Filesystem.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>


NAS2D::Point<int> MOUSE_COORDS; /**< Mouse Coordinates. Used by other states/wrapers. */


GameState::GameState():
	mMainReportsState{std::make_unique<MainReportsUiState>()}
{
	mMainReportsState->_initialize();
	mMainReportsState->hideReports().connect({this, &GameState::onHideReports});

	for (auto takeMeThere : mMainReportsState->takeMeThere())
	{
		takeMeThere->connect({this, &GameState::onTakeMeThere});
	}

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().connect({this, &GameState::onMouseMove});

	NAS2D::Utility<NAS2D::Mixer>::get().musicCompleteSignalSource().connect({this, &GameState::onMusicComplete});
}


GameState::GameState(const std::string& savedGameFilename) : GameState()
{
	mMapView = std::make_unique<MapViewState>(*mMainReportsState.get(), savedGameFilename);
	initializeMapViewState();
}


GameState::GameState(const Planet::Attributes& planetAttributes, Difficulty selectedDifficulty) : GameState()
{
	mMapView = std::make_unique<MapViewState>(*mMainReportsState.get(), planetAttributes, selectedDifficulty);
	mMapView->setPopulationLevel(MapViewState::PopulationLevel::Large);
	initializeMapViewState();
}


GameState::~GameState()
{
	NAS2D::Utility<StructureManager>::get().dropAllStructures();

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().disconnect({this, &GameState::onMouseMove});

	NAS2D::Utility<NAS2D::Mixer>::get().musicCompleteSignalSource().disconnect({this, &GameState::onMusicComplete});
	NAS2D::Utility<NAS2D::Mixer>::get().stopAllAudio();
}


void GameState::initialize()
{
	mFade.fadeIn(constants::FadeSpeed);
}


void GameState::initializeMapViewState()
{
	mActiveState = mMapView.get();

	mMapView->_initialize();
	mMapView->activate();

	mMapView->quit().connect({this, &GameState::onQuit});
	mMapView->showReportsUi().connect({this, &GameState::onShowReports});
	mMapView->mapChanged().connect({this, &GameState::onMapChange});
	mMapView->fileLoadSignal().connect({this, &GameState::onLoadGame});
}


void GameState::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	MOUSE_COORDS = position;
}


/**
 * Music Complete event handler.
 *
 * Called by NAS2D::Mixer upon completion of a music track. This function
 * changes the background music track to a different track in the lineup.
 */
void GameState::onMusicComplete()
{
	/// \todo	Make me work... once there's some music to listen to. 0.0
}


/**
 * Event handler that responds to a quit event raised by the MapViewState.
 *
 * This event is raised on game overs and when the user chooses the "Return
 * to Main Menu" from the system options window.
 */
void GameState::onQuit()
{
	mMapView->deactivate();
	mMainReportsState->deactivate();
}


/**
 * Event handler that responds to a show reports event raised by the MapViewState.
 *
 * This event is raised whenever a user double-clicks on a factory in the MapViewState.
 */
void GameState::onShowReports()
{
	if (mMainReportsState)
	{
		mActiveState->deactivate();
		mActiveState = mMainReportsState.get();
		mActiveState->activate();
	}
}


/**
 * Event handler that responds to a hide report event raised by the MainReportsUiState.
 *
 * This event is raised by the MainReportsUiState whenever the user clicks the Exit
 * UI panel or if the Escape key is pressed.
 */
void GameState::onHideReports()
{
	if (mMapView)
	{
		mActiveState->deactivate();
		mActiveState = mMapView.get();
		mActiveState->activate();
	}
}


void GameState::onMapChange()
{
	mMainReportsState->clearLists();
}


void GameState::onLoadGame(const std::string& saveGameName)
{
	auto& filesystem = NAS2D::Utility<NAS2D::Filesystem>::get();
	auto saveGamePath = constants::SaveGamePath + saveGameName + ".xml";
	try
	{
		if (!filesystem.exists(saveGamePath))
		{
			throw std::runtime_error("Save game file does not exist: " + saveGamePath);
		}
		mNewMapView = std::make_unique<MapViewState>(*mMainReportsState.get(), saveGamePath);
	}
	catch (const std::exception& e)
	{
		doNonFatalErrorMessage("Load Failed", e.what());
		return;
	}
	mNewMapView->_initialize();
}


/**
 * Event handler that responds to a 'take me there' event raised by the MainReportsUiState.
 *
 * This event is raised by the MainReportsUiState whenever a "Take Me There" button in any
 * of the report UI panels is clicked.
 */
void GameState::onTakeMeThere(const Structure* structure)
{
	onHideReports();
	mMapView->focusOnStructure(structure);
}


NAS2D::State* GameState::update()
{
	if (mActiveState)
	{
		mActiveState->_update();
	}

	mFade.update();
	mFade.draw(NAS2D::Utility<NAS2D::Renderer>::get());

	if (mMapView && mMapView->hasGameEnded())
	{
		mReturnState = new MainMenuState();
	}

	if (mNewMapView)
	{
		mMapView = std::move(mNewMapView);
		mActiveState = mMapView.get();
		mActiveState->activate();
		mMapView->quit().connect({this, &GameState::onQuit});
		mMapView->showReportsUi().connect({this, &GameState::onShowReports});
		mMapView->mapChanged().connect({this, &GameState::onMapChange});
		mMapView->fileLoadSignal().connect({this, &GameState::onLoadGame});
	}

	return mReturnState;
}
