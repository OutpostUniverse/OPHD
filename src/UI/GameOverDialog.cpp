// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GameOverDialog.h"

#include "../Constants.h"


GameOverDialog::GameOverDialog(Font& font) : mHeader("ui/interface/game_over.png")
{
	Control::font(font);
	//text(constants::WINDOW_GAME_OVER);
	init();
}


GameOverDialog::~GameOverDialog()
{}


void GameOverDialog::init()
{
	position(0, 0);
	size(522, 340);

	add(&btnClose, 5, 310);
	btnClose.font(font());
	btnClose.text("Return to Main Menu");
	btnClose.size(512, 25);
	btnClose.click().connect(this, &GameOverDialog::btnCloseClicked);

	anchored(true);
}


void GameOverDialog::update()
{
	if (!visible())
		return;

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawImage(mHeader, rect().x() + 5, rect().y() + 25);

	r.drawText(font(), "You have failed. Your colony is dead.", rect().x() + 5, rect().y() + 290, 255, 255, 255);
}


void GameOverDialog::btnCloseClicked()
{
	mCallback();
}
