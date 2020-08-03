// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GameOverDialog.h"

#include "../Constants.h"
#include "../FontManager.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

using namespace NAS2D;


GameOverDialog::GameOverDialog() :
	mHeader{"ui/interface/game_over.png"},
	btnClose{"Return to Main Menu"}
{
	init();
}


GameOverDialog::~GameOverDialog()
{}


void GameOverDialog::init()
{
	position({0, 0});
	size({522, 340});

	add(&btnClose, 5, 310);
	btnClose.size({512, 25});
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

	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(mHeader, position() + NAS2D::Vector{5, 25});

	// Yeah, I know. I hate it too but it made more sense than holding onto a static pointer.
	const auto& font = *&Utility<FontManager>::get().load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	renderer.drawText(font, "You have failed. Your colony is dead.", position() + NAS2D::Vector{5, 290}, NAS2D::Color::White);
}
