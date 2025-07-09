#include "GameOptionsDialog.h"

#include "../Constants/Strings.h"
#include "../ShellOpenPath.h"

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>

#include <array>


namespace
{
	constexpr auto buttonSize = NAS2D::Vector{200, 25};
	constexpr auto buttonMargin = NAS2D::Vector{5, 3};
}


GameOptionsDialog::GameOptionsDialog(
	ClickHandler saveClickHandler,
	ClickHandler loadClickHandler,
	ClickHandler exitClickHandler,
	ClickHandler continueClickHandler
) :
	Window{constants::WindowSystemTitle},
	mContinueClickHandler{continueClickHandler},
	btnSave{"Save Game", saveClickHandler},
	btnLoad{"Load Game", loadClickHandler},
	btnHelp{"Help", {this, &GameOptionsDialog::onHelp}},
	btnExit{"Exit to Main Menu", exitClickHandler},
	btnContinue{"Continue Playing", continueClickHandler}
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

	NAS2D::Utility<NAS2D::EventHandler>::get().keyDown().connect({this, &GameOptionsDialog::onKeyDown});
}


GameOptionsDialog::~GameOptionsDialog()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().keyDown().disconnect({this, &GameOptionsDialog::onKeyDown});
}


void GameOptionsDialog::onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier /*mod*/, bool /*repeat*/)
{
	if (key == NAS2D::KeyCode::Escape) { mContinueClickHandler(); }
}


void GameOptionsDialog::onEnableChange()
{
	btnSave.enabled(enabled());
	btnLoad.enabled(enabled());
	btnHelp.enabled(enabled());
	btnExit.enabled(enabled());
	btnContinue.enabled(enabled());
}


void GameOptionsDialog::onHelp()
{
	shellOpenPath("https://wiki.outpost2.net/doku.php?id=outposthd:how_to_play");
}
