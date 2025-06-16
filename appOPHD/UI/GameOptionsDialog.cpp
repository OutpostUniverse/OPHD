#include "GameOptionsDialog.h"

#include "../Constants/Strings.h"
#include "../ShellOpenPath.h"
#include <array>


namespace
{
	constexpr auto buttonSize = NAS2D::Vector{200, 25};
	constexpr auto buttonMargin = NAS2D::Vector{5, 3};
}


GameOptionsDialog::GameOptionsDialog() :
	Window{constants::WindowSystemTitle},
	btnSave{"Save Game", {this, &GameOptionsDialog::onSave}},
	btnLoad{"Load Game", {this, &GameOptionsDialog::onLoad}},
	btnHelp{"Help", {this, &GameOptionsDialog::onHelp}},
	btnExit{"Exit to Main Menu", {this, &GameOptionsDialog::onExit}},
	btnContinue{"Continue Playing", {this, &GameOptionsDialog::onContinue}}
{
	position({0, 0});

	const auto buttons = std::array{&btnSave, &btnLoad, &btnHelp, &btnExit, &btnContinue};
	auto position = mRect.position + NAS2D::Vector{buttonMargin.x, buttonSize.y};
	for (auto button : buttons)
	{
		button->size(buttonSize);
		add(*button, {position.x, position.y});
		position.y += buttonSize.y + buttonMargin.y;
	}

	size({buttonSize.x + 2 * buttonMargin.x, position.y});

	anchored(true);
}


void GameOptionsDialog::onEnableChange()
{
	btnSave.enabled(enabled());
	btnLoad.enabled(enabled());
	btnHelp.enabled(enabled());
	btnExit.enabled(enabled());
	btnContinue.enabled(enabled());
}


void GameOptionsDialog::update()
{
	if (!visible()) { return; }

	Window::update();
}

void GameOptionsDialog::onSave()
{
	mSignalSaveDialog();
}

void GameOptionsDialog::onLoad()
{
	mSignalLoadDialog();
}

void GameOptionsDialog::onHelp()
{
	shellOpenPath("https://wiki.outpost2.net/doku.php?id=outposthd:how_to_play");
}

void GameOptionsDialog::onExit()
{
	mSignalExit();
}

void GameOptionsDialog::onContinue()
{
	mSignalContinue();
}
