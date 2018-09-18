// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GameState.h"

#include "MainMenuState.h"
#include "MapViewState.h"
#include "MainReportsUiState.h"

#include "Wrapper.h"

Point_2d MOUSE_COORDS;
MainReportsUiState* MAIN_REPORTS_UI = nullptr;


GameState::GameState()
{}


GameState::~GameState()
{
	EventHandler& e = Utility<EventHandler>::get();

	e.mouseMotion().disconnect(this, &GameState::onMouseMotion);
	e.mouseButtonDown().disconnect(this, &GameState::onMouseDown);
	e.mouseButtonUp().disconnect(this, &GameState::onMouseUp);

	Utility<Renderer>::get().fadeComplete().disconnect(this, &GameState::fadeComplete);

	WrapperStack& _stack = Utility<WrapperStack>::get();
	while (!_stack.empty())
	{
		_stack.top()->deactivate();
		if (_stack.top() == MAIN_REPORTS_UI) { MAIN_REPORTS_UI = nullptr; } // kludge
		delete _stack.top();
		_stack.pop();
	}

	delete MAIN_REPORTS_UI;

	Utility<Mixer>::get().musicComplete().disconnect(this, &GameState::musicComplete);
	Utility<StructureManager>::get().dropAllStructures();
}


void GameState::initialize()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.mouseMotion().connect(this, &GameState::onMouseMotion);
	e.mouseButtonDown().connect(this, &GameState::onMouseDown);
	e.mouseButtonUp().connect(this, &GameState::onMouseUp);


	MAIN_REPORTS_UI = new MainReportsUiState();
	MAIN_REPORTS_UI->_initialize();

	Utility<Renderer>::get().fadeComplete().connect(this, &GameState::fadeComplete);
	Utility<Renderer>::get().fadeIn(constants::FADE_SPEED);
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


State* GameState::update()
{
	Renderer& r = Utility<Renderer>::get();

	WrapperStack& _stack = Utility<WrapperStack>::get();

	if (_stack.empty())
	{
		throw std::runtime_error("Stack is empty.");
	}

	State* _s = _stack.top()->_update();
	if (!_s && _stack.size() == 1)
	{
		if (!r.isFading())
		{
			r.fadeOut(constants::FADE_SPEED);
			Utility<Mixer>::get().fadeOutMusic(constants::FADE_SPEED);
		}
	}
	else if (!_s && _stack.size() > 1)
	{
		_stack.top()->deactivate();
		_stack.pop();
		_stack.top()->activate();
	}
	else if (_s != _stack.top())
	{
		_stack.top()->deactivate();
		_stack.push(static_cast<Wrapper*>(_s));
		_stack.top()->activate();
	}

	return mReturnState;
}
