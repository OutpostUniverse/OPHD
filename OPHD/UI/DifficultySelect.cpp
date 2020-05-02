// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "DifficultySelect.h"

#include "NAS2D/Renderer/Renderer.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Mixer/Mixer.h"
#include "NAS2D/StringUtils.h"

#include "../Constants.h"
#include "../Common.h"

#include <sstream>

#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"


DifficultySelect::DifficultySelect() :
	rdbBeginner{constants::DIFFICULTY_BEGINNER},
	rdbEasy{constants::DIFFICULTY_EASY},
	rdbMedium{constants::DIFFICULTY_MEDIUM},
	rdbHard{constants::DIFFICULTY_HARD},
	btnOk{"Ok"}
{
	text(constants::DIFFICULTY_TITLE);
	init();
}

DifficultySelect::~DifficultySelect()
{
	auto& e = NAS2D::Utility<NAS2D::EventHandler>::get();
	e.keyDown().disconnect(this, &DifficultySelect::onKeyDown);

	btnOk.click().disconnect(this, &DifficultySelect::btnOkClicked);
}

void DifficultySelect::init()
{
	auto& e = NAS2D::Utility<NAS2D::EventHandler>::get();
	e.keyDown().connect(this, &DifficultySelect::onKeyDown);

	size({150, 145});

	const auto border_left_width = 5;
	const auto border_right_width = 5;
	constexpr auto border_top_height = sWindowTitleBarHeight + 5;
	const auto border_bottom_height = 5;
	const auto element_spacing = 5;
	const auto left_edge = positionX() + border_left_width;
	const auto right_edge = left_edge + width() - border_right_width * 2.0f;
	const auto top_edge = positionY() + border_top_height;
	const auto bottom_edge = top_edge + height() - border_top_height - border_bottom_height * 2.0f;
	const auto width = right_edge - left_edge;

	btnOk.size({100, 25});
	btnOk.click().connect(this, &DifficultySelect::btnOkClicked);

	switch(CURRENT_DIFFICULTY)
	{
	case Difficulty::Beginner:  rdbBeginner.checked(true); break;
	case Difficulty::Easy:      rdbEasy.checked(true);     break;
	case Difficulty::Medium:    rdbMedium.checked(true);   break;
	case Difficulty::Hard:      rdbHard.checked(true);     break;
	default:                    rdbBeginner.checked(true); break;
	}

	float longest_width = 0.0f;
	if (longest_width < rdbBeginner.width())
	{
		longest_width = rdbBeginner.width();
	}
	if (longest_width < rdbEasy.width())
	{
		longest_width = rdbEasy.width();
	}
	if (longest_width < rdbMedium.width())
	{
		longest_width = rdbMedium.width();
	}
	if (longest_width < rdbHard.width())
	{
		longest_width = rdbHard.width();
	}

	rdbBeginner.width(longest_width);
	rdbEasy.width(longest_width);
	rdbMedium.width(longest_width);
	rdbHard.width(longest_width);


	add(&rdbBeginner, width / 2 - rdbBeginner.width() / 2, top_edge + element_spacing);
	add(&rdbEasy, width / 2 - rdbEasy.width() / 2, rdbBeginner.rect().endPoint().y() + element_spacing);
	add(&rdbMedium, width / 2 - rdbMedium.width() / 2, rdbEasy.rect().endPoint().y() + element_spacing);
	add(&rdbHard, width / 2 - rdbHard.width() / 2, rdbMedium.rect().endPoint().y() + element_spacing);

	add(&btnOk, width / 2 - btnOk.width() / 2, bottom_edge - element_spacing - btnOk.height());

	anchored(true);
	hide();

}

void DifficultySelect::onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier /*mod*/, bool /*repeat*/) {
	if(key == NAS2D::EventHandler::KeyCode::KEY_ESCAPE) {
		cancelled();
	}
}


void DifficultySelect::visibilityChanged(bool visible) {
	if(visible)
	{
		enableControls();
	}
	else
	{
		disableControls();
	}
}

void DifficultySelect::btnOkClicked() {
	if(rdbBeginner.checked())
	{
		CURRENT_DIFFICULTY = Difficulty::Beginner;
	}
	else if(rdbEasy.checked())
	{
		CURRENT_DIFFICULTY = Difficulty::Easy;
	}
	else if(rdbMedium.checked())
	{
		CURRENT_DIFFICULTY = Difficulty::Medium;
	}
	else if(rdbHard.checked())
	{
		CURRENT_DIFFICULTY = Difficulty::Hard;
	}
	mOkCallback();
}

void DifficultySelect::cancelled() {
	hide();
	mCancelCallback();
}

void DifficultySelect::enableControls() {
	rdbBeginner.enabled(true);
	rdbEasy.enabled(true);
	rdbMedium.enabled(true);
	rdbHard.enabled(true);
	btnOk.enabled(true);
}

void DifficultySelect::disableControls() {
	rdbBeginner.enabled(false);
	rdbEasy.enabled(false);
	rdbMedium.enabled(false);
	rdbHard.enabled(false);
	btnOk.enabled(false);
}

void DifficultySelect::update()
{
	if(!visible()) { return; }
	Window::update();
}
