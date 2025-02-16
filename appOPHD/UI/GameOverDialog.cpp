#include "GameOverDialog.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


GameOverDialog::GameOverDialog() :
	mHeader{imageCache.load("ui/interface/game_over.png")},
	btnClose{"Return to Main Menu", {this, &GameOverDialog::onClose}}
{
	position({0, 0});
	size({522, 340});

	add(btnClose, {5, 310});
	btnClose.size({512, 25});

	anchored(true);
}


void GameOverDialog::onClose()
{
	mSignal();
}


void GameOverDialog::update()
{
	if (!visible()) { return; }

	Window::update();

	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(mHeader, position() + NAS2D::Vector{5, 25});

	const auto& font = Control::getDefaultFont();
	renderer.drawText(font, "You have failed. Your colony is dead.", position() + NAS2D::Vector{5, 290}, NAS2D::Color::White);
}
