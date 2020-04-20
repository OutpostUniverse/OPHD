// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GameState.h"

#include "MainMenuState.h"
#include "MapViewState.h"
#include "MainReportsUiState.h"

#include "Wrapper.h"

#include "NAS2D/Mixer/Mixer.h"
#include "NAS2D/Renderer/Renderer.h"

NAS2D::Point<int> MOUSE_COORDS; /**< Mouse Coordinates. Used by other states/wrapers. */

MainReportsUiState* MAIN_REPORTS_UI = nullptr; /**< Pointer to a MainReportsUiState. Memory is handled by GameState. */
static MapViewState* MAP_VIEW = nullptr; /**< Pointer to a MapViewState. Memory is handled by GameState. */

static Wrapper* ACTIVE_STATE = nullptr; /**< The currently active State. */


/**
 * C'tor
 */
GameState::GameState()
{}


/**
 * D'tor
 */
GameState::~GameState()
{
	NAS2D::Utility<StructureManager>::get().dropAllStructures();

	NAS2D::EventHandler& e = NAS2D::Utility<NAS2D::EventHandler>::get();
	e.mouseMotion().disconnect(this, &GameState::onMouseMove);

	NAS2D::Utility<NAS2D::Renderer>::get().fadeComplete().disconnect(this, &GameState::fadeComplete);

	MAIN_REPORTS_UI->hideReports().disconnect(this, &GameState::hideReportsUi);
	MAP_VIEW->quit().disconnect(this, &GameState::quitEvent);
	MAP_VIEW->showReporstUi().disconnect(this, &GameState::showReportsUi);
	MAP_VIEW->mapChanged().disconnect(this, &GameState::mapChanged);

	for (auto takeMeThere : MAIN_REPORTS_UI->takeMeThere())
	{
		takeMeThere->disconnect(this, &GameState::takeMeThere);
	}

	delete MAIN_REPORTS_UI;
	delete MAP_VIEW;

	NAS2D::Utility<NAS2D::Mixer>::get().removeMusicCompleteHandler(MakeDelegate(this, &GameState::musicComplete));
	NAS2D::Utility<NAS2D::Mixer>::get().stopAllAudio();
}


/**
 * Internal initializer function.
 */
void GameState::initialize()
{
	NAS2D::EventHandler& e = NAS2D::Utility<NAS2D::EventHandler>::get();
	e.mouseMotion().connect(this, &GameState::onMouseMove);

	MAIN_REPORTS_UI = new MainReportsUiState();
	MAIN_REPORTS_UI->_initialize();
	MAIN_REPORTS_UI->hideReports().connect(this, &GameState::hideReportsUi);

	for (auto takeMeThere : MAIN_REPORTS_UI->takeMeThere())
	{
		takeMeThere->connect(this, &GameState::takeMeThere);
	}

	NAS2D::Utility<NAS2D::Mixer>::get().addMusicCompleteHandler(MakeDelegate(this, &GameState::musicComplete));
	NAS2D::Utility<NAS2D::Renderer>::get().fadeComplete().connect(this, &GameState::fadeComplete);
	NAS2D::Utility<NAS2D::Renderer>::get().fadeIn(constants::FADE_SPEED);
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
	MAP_VIEW = state;
	ACTIVE_STATE = MAP_VIEW;

	MAP_VIEW->quit().connect(this, &GameState::quitEvent);
	MAP_VIEW->showReporstUi().connect(this, &GameState::showReportsUi);
	MAP_VIEW->mapChanged().connect(this, &GameState::mapChanged);
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
void GameState::fadeComplete()
{
	NAS2D::Renderer& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
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
void GameState::musicComplete()
{
	/// \todo	Make me work... once there's some music to listen to. 0.0
}


/**
 * Event handler that responds to a quit event raised by the MapViewState.
 * 
 * This event is raised on game overs and when the user chooses the "Return
 * to Main Menu" from the system options window.
 */
void GameState::quitEvent()
{
	MAP_VIEW->deactivate();
	MAIN_REPORTS_UI->deactivate();
}


/**
 * Event handler that responds to a show reports event raised by the MapViewState.
 * 
 * This event is raised whenever a user double-clicks on a factory in the MapViewState.
 */
void GameState::showReportsUi()
{
	ACTIVE_STATE->deactivate();
	ACTIVE_STATE = MAIN_REPORTS_UI;
	ACTIVE_STATE->activate();
}


/**
 * Event handler that responds to a hide report event raised by the MainReportsUiState.
 * 
 * This event is raised by the MainReportsUiState whenever the user clicks the Exit
 * UI panel or if the Escape key is pressed.
 */
void GameState::hideReportsUi()
{
	ACTIVE_STATE->deactivate();
	ACTIVE_STATE = MAP_VIEW;
	ACTIVE_STATE->activate();
}


void GameState::mapChanged()
{
	MAIN_REPORTS_UI->clearLists();
}


/**
 * Event handler that responds to a 'take me there' event raised by the MainReportsUiState.
 * 
 * This event is raised by the MainReportsUiState whenever a "Take Me There" button in any
 * of the report UI panels is clicked.
 */
void GameState::takeMeThere(Structure* _s)
{
	hideReportsUi();
	MAP_VIEW->focusOnStructure(_s);
}


/**
 * Update
 */
NAS2D::State* GameState::update()
{
	if (ACTIVE_STATE)
	{
		ACTIVE_STATE->_update();
	}

	return mReturnState;
}
