#include "GameState.h"

#include "MainMenuState.h"
#include "MapViewState.h"
#include "MainReportsUiState.h"
#include "Wrapper.h"
#include "../StructureManager.h"
#include "../UI/MessageBox.h"

#include <NAS2D/Filesystem.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>


NAS2D::Point<int> MOUSE_COORDS; /**< Mouse Coordinates. Used by other states/wrapers. */


// Explicit constructor needed in implementation file
// If a default constructor was used instead, construction would happen in importing translation unit
// The header uses forward declares for some types, so only incomplete types are available to importing code
// Complete types are needed to construct the std::unique_ptr members
GameState::GameState():
	mMainReportsState{std::make_unique<MainReportsUiState>()}
{
	mMainReportsState->_initialize();
	mMainReportsState->hideReports().connect({this, &GameState::onHideReports});

	for (auto takeMeThere : mMainReportsState->takeMeThere())
	{
		takeMeThere->connect({this, &GameState::onTakeMeThere});
	}

}


GameState::~GameState()
{
	NAS2D::Utility<StructureManager>::get().dropAllStructures();

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().disconnect({this, &GameState::onMouseMove});

	mMainReportsState->hideReports().disconnect({this, &GameState::onHideReports});
	mMapView->quit().disconnect({this, &GameState::onQuit});
	mMapView->showReportsUi().disconnect({this, &GameState::onShowReports});
	mMapView->mapChanged().disconnect({this, &GameState::onMapChange});

	for (auto takeMeThere : mMainReportsState->takeMeThere())
	{
		takeMeThere->disconnect({this, &GameState::onTakeMeThere});
	}

	NAS2D::Utility<NAS2D::Mixer>::get().musicCompleteSignalSource().disconnect({this, &GameState::onMusicComplete});
	NAS2D::Utility<NAS2D::Mixer>::get().stopAllAudio();
}


/**
 * Internal initializer function.
 */
void GameState::initialize()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().connect({this, &GameState::onMouseMove});

	NAS2D::Utility<NAS2D::Mixer>::get().musicCompleteSignalSource().connect({this, &GameState::onMusicComplete});
	mFade.fadeIn(constants::FadeSpeed);
}


/**
 * Sets a pointer for the MapViewState.
 *
 * Since the MapViewState is created outside of the GameState, this function
 * takes a pointer to an already instatiated MapViewState object.
 *
 * \param	state	Pointer to a MapViewState. Ownership is transfered to GameState.
 *
 * \note	GameState will handle correct destruction of the MapViewState object.
 */
void GameState::mapviewstate(MapViewState* state)
{
	mMapView.reset(state);
	mActiveState = mMapView.get();

	mMapView->quit().connect({this, &GameState::onQuit});
	mMapView->showReportsUi().connect({this, &GameState::onShowReports});
	mMapView->mapChanged().connect({this, &GameState::onMapChange});
	mMapView->fileLoadSignal().connect({this, &GameState::onLoadGame});
}


MainReportsUiState& GameState::getMainReportsState()
{
	return *mMainReportsState;
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
		mNewMapView = std::make_unique<MapViewState>(getMainReportsState(), saveGamePath);
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

	return mReturnState;
}
