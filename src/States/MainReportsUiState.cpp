// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MainReportsUiState.h"

using namespace NAS2D;

MainReportsUiState::MainReportsUiState()
{}


MainReportsUiState::~MainReportsUiState()
{}


void MainReportsUiState::_deactivate()
{
	Utility<EventHandler>::get().keyDown().disconnect(this, &MainReportsUiState::onKeyDown);
}


void MainReportsUiState::_activate()
{
	Utility<EventHandler>::get().keyDown().connect(this, &MainReportsUiState::onKeyDown);
	mReturnState = this;
}


void MainReportsUiState::initialize()
{

}


void MainReportsUiState::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat)
{
	if (key == EventHandler::KEY_ESCAPE) { mReturnState = nullptr; }
}


State* MainReportsUiState::update()
{
	Utility<Renderer>::get().clearScreen(255, 0, 255);
	return mReturnState;
}
