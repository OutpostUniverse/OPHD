// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GameState.h"

#include "MainMenuState.h"
#include "MapViewState.h"
#include "MainReportsUiState.h"

#include "Wrapper.h"

Point_2d MOUSE_COORDS;

MainReportsUiState* MAIN_REPORTS_UI = nullptr;
MapViewState* MAP_VIEW = nullptr;

Wrapper* ACTIVE_STATE = nullptr;


GameState::GameState()
{}


GameState::~GameState()
{
	Utility<StructureManager>::get().dropAllStructures();

	EventHandler& e = Utility<EventHandler>::get();

	e.mouseMotion().disconnect(this, &GameState::onMouseMotion);
	e.mouseButtonDown().disconnect(this, &GameState::onMouseDown);
	e.mouseButtonUp().disconnect(this, &GameState::onMouseUp);

	Utility<Renderer>::get().fadeComplete().disconnect(this, &GameState::fadeComplete);

	MAIN_REPORTS_UI->hideReports().disconnect(this, &GameState::hideReportsUi);
	MAP_VIEW->quit().disconnect(this, &GameState::quitEvent);
	MAP_VIEW->showReporstUi().disconnect(this, &GameState::showReportsUi);

	std::cout << std::endl << std::endl << "******* Deleteing GameState's sub states... ";

	delete MAIN_REPORTS_UI;
	delete MAP_VIEW;

	std::cout << std::endl << std::endl << "******* done. Backing out." << std::endl;

	Utility<Mixer>::get().musicComplete().disconnect(this, &GameState::musicComplete);
	Utility<Mixer>::get().stopAllAudio();
}


void GameState::initialize()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.mouseMotion().connect(this, &GameState::onMouseMotion);
	e.mouseButtonDown().connect(this, &GameState::onMouseDown);
	e.mouseButtonUp().connect(this, &GameState::onMouseUp);

	MAIN_REPORTS_UI = new MainReportsUiState();
	MAIN_REPORTS_UI->_initialize();
	MAIN_REPORTS_UI->hideReports().connect(this, &GameState::hideReportsUi);

	Utility<Renderer>::get().fadeComplete().connect(this, &GameState::fadeComplete);
	Utility<Renderer>::get().fadeIn(constants::FADE_SPEED);
}


void GameState::mapviewstate(MapViewState* state)
{
	MAP_VIEW = state;
	ACTIVE_STATE = MAP_VIEW;

	MAP_VIEW->quit().connect(this, &GameState::quitEvent);
	MAP_VIEW->showReporstUi().connect(this, &GameState::showReportsUi);
}


void GameState::onMouseMotion(int x, int y, int relX, int relY)
{
	MOUSE_COORDS(x, y);
}


void GameState::onMouseDown(EventHandler::MouseButton, int x, int y)
{}


void GameState::onMouseUp(EventHandler::MouseButton, int x, int y)
{}


void GameState::fadeComplete()
{
	Renderer& r = Utility<Renderer>::get();
	if (r.isFaded())
	{
		mReturnState = new MainMenuState();
	}
}


void GameState::musicComplete()
{}


void GameState::quitEvent()
{
	std::cout << std::endl << std::endl << "******* Quit event received... ";

	MAP_VIEW->deactivate();
	MAIN_REPORTS_UI->deactivate();
}


void GameState::showReportsUi()
{
	ACTIVE_STATE->deactivate();
	ACTIVE_STATE = MAIN_REPORTS_UI;
	ACTIVE_STATE->activate();
}


void GameState::hideReportsUi()
{
	ACTIVE_STATE->deactivate();
	ACTIVE_STATE = MAP_VIEW;
	ACTIVE_STATE->activate();
}



State* GameState::update()
{
	Renderer& r = Utility<Renderer>::get();

	if (ACTIVE_STATE)
	{
		ACTIVE_STATE->_update();
	}

	return mReturnState;
}
