#include "GameOverDialog.h"

#include "../Cache.h"
#include "../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


GameOverDialog::GameOverDialog() :
	mHeader{imageCache.load("ui/interface/game_over.png")},
	btnClose{"Return to Main Menu"}
{
	position({0, 0});
	size({522, 340});

	add(btnClose, {5, 310});
	btnClose.size({512, 25});
	btnClose.click().connect(this, &GameOverDialog::onClose);

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

	const auto& font = fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	renderer.drawText(font, "You have failed. Your colony is dead.", position() + NAS2D::Vector{5, 290}, NAS2D::Color::White);
}
