// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GameOverDialog.h"

#include "../Constants.h"
#include "../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"

using namespace NAS2D;


GameOverDialog::GameOverDialog() : mHeader("ui/interface/game_over.png")
{
	init();
}


GameOverDialog::~GameOverDialog()
{}


void GameOverDialog::init()
{
	position(0, 0);
	size(522, 340);

	add(&btnClose, 5, 310);
	btnClose.text("Return to Main Menu");
	btnClose.size(512, 25);
	btnClose.click().connect(this, &GameOverDialog::btnCloseClicked);

	anchored(true);
}


void GameOverDialog::btnCloseClicked()
{
	mCallback();
}


void GameOverDialog::update()
{
	if (!visible()) { return; }

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawImage(mHeader, rect().x() + 5, rect().y() + 25);

	// Yeah, I know. I hate it too but it made more sense than holding onto a static pointer.
	r.drawText(	*Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL),
				"You have failed. Your colony is dead.", rect().x() + 5, rect().y() + 290, 255, 255, 255);
}
