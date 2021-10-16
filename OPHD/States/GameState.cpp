#include "GameState.h"

#include "MainMenuState.h"
#include "MapViewState.h"
#include "MainReportsUiState.h"
#include "Wrapper.h"
#include "../StructureManager.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>


NAS2D::Point<int> MOUSE_COORDS; /**< Mouse Coordinates. Used by other states/wrapers. */


// Explicit constructor needed in implementation file
// If a default constructor was used instead, construction would happen in importing translation unit
// The header uses forward declares for some types, so only incomplete types are available to importing code
// Complete types are needed to construct the std::unique_ptr members
GameState::GameState()
{}


GameState::~GameState()
{
	NAS2D::Utility<StructureManager>::get().dropAllStructures();

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().disconnect(this, &GameState::onMouseMove);

	NAS2D::Utility<NAS2D::Renderer>::get().fadeComplete().disconnect(this, &GameState::onFadeComplete);

	mMainReportsState->hideReports().disconnect(this, &GameState::onHideReports);
	mMapView->quit().disconnect(this, &GameState::onQuit);
	mMapView->showReporstUi().disconnect(this, &GameState::onShowReports);
	mMapView->mapChanged().disconnect(this, &GameState::onMapChange);

	for (auto takeMeThere : mMainReportsState->takeMeThere())
	{
		takeMeThere->disconnect(this, &GameState::onTakeMeThere);
	}

	NAS2D::Utility<NAS2D::Mixer>::get().musicCompleteSignalSource().disconnect(MakeDelegate(this, &GameState::onMusicComplete));
	NAS2D::Utility<NAS2D::Mixer>::get().stopAllAudio();
}


/**
 * Internal initializer function.
 */
void GameState::initialize()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseMotion().connect(this, &GameState::onMouseMove);

	mMainReportsState = std::make_unique<MainReportsUiState>();
	mMainReportsState->_initialize();
	mMainReportsState->hideReports().connect(this, &GameState::onHideReports);

	for (auto takeMeThere : mMainReportsState->takeMeThere())
	{
		takeMeThere->connect(this, &GameState::onTakeMeThere);
	}

	NAS2D::Utility<NAS2D::Mixer>::get().musicCompleteSignalSource().connect(MakeDelegate(this, &GameState::onMusicComplete));
	NAS2D::Utility<NAS2D::Renderer>::get().fadeComplete().connect(this, &GameState::onFadeComplete);
	NAS2D::Utility<NAS2D::Renderer>::get().fadeIn(constants::FadeSpeed);
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

	mMapView->quit().connect(this, &GameState::onQuit);
	mMapView->showReporstUi().connect(this, &GameState::onShowReports);
	mMapView->mapChanged().connect(this, &GameState::onMapChange);
}


MainReportsUiState& GameState::getMainReportsState()
{
	return *mMainReportsState;
}


/**
 * Mouse motion event handler.
 */
void GameState::onMouseMove(int x, int y, int /*relX*/, int /*relY*/)
{
	MOUSE_COORDS = {x, y};
}


/**
 * Event hanler for a 'fade complete' event raised by the NAS2D::Renderer.
 */
void GameState::onFadeComplete()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	if (renderer.isFaded())
	{
		mReturnState = new MainMenuState();
	}
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


/**
 * Event handler that responds to a 'take me there' event raised by the MainReportsUiState.
 * 
 * This event is raised by the MainReportsUiState whenever a "Take Me There" button in any
 * of the report UI panels is clicked.
 */
void GameState::onTakeMeThere(Structure* structure)
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

	return mReturnState;
}
