// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "PopulationPanel.h"

#include "../Constants.h"
#include "../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"

using namespace NAS2D;

static Font* FONT = nullptr;

PopulationPanel::PopulationPanel() : mIcons("ui/icons.png")
{
	size(160, 220);

	mSkin.push_back(Image("ui/skin/window_top_left.png"));
	mSkin.push_back(Image("ui/skin/window_top_middle.png"));
	mSkin.push_back(Image("ui/skin/window_top_right.png"));
	mSkin.push_back(Image("ui/skin/window_middle_left.png"));
	mSkin.push_back(Image("ui/skin/window_middle_middle.png"));
	mSkin.push_back(Image("ui/skin/window_middle_right.png"));
	mSkin.push_back(Image("ui/skin/window_bottom_left.png"));
	mSkin.push_back(Image("ui/skin/window_bottom_middle.png"));
	mSkin.push_back(Image("ui/skin/window_bottom_right.png"));

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
}


void PopulationPanel::update()
{
	Renderer& r = Utility<Renderer>::get();
	r.drawImageRect(rect(), mSkin);

	auto position = NAS2D::Point{positionX() + 5, positionY() + 5};
	r.drawText(*FONT, string_format("Morale: %i", *mMorale), position, NAS2D::Color::White);
	position.y() += 10;
	r.drawText(*FONT, string_format("Previous: %i", *mPreviousMorale), position, NAS2D::Color::White);

	mCapacity = (mResidentialCapacity > 0) ? (static_cast<float>(mPopulation->size()) / static_cast<float>(mResidentialCapacity)) * 100.0f : 0.0f;
	
	position.y() += 15;
	const auto text = string_format("Housing: %i / %i  (%i%%)", mPopulation->size(), mResidentialCapacity, static_cast<int>(mCapacity));
	r.drawText(*FONT, text, position, NAS2D::Color::White);

	const std::array populationData{
		std::pair{NAS2D::Rectangle{0, 96, 32, 32}, mPopulation->size(Population::ROLE_CHILD)},
		std::pair{NAS2D::Rectangle{32, 96, 32, 32}, mPopulation->size(Population::ROLE_STUDENT)},
		std::pair{NAS2D::Rectangle{64, 96, 32, 32}, mPopulation->size(Population::ROLE_WORKER)},
		std::pair{NAS2D::Rectangle{96, 96, 32, 32}, mPopulation->size(Population::ROLE_SCIENTIST)},
		std::pair{NAS2D::Rectangle{128, 96, 32, 32}, mPopulation->size(Population::ROLE_RETIRED)},
	};

	position.y() += 15;
	const auto fontOffset = NAS2D::Vector{37, 32 - FONT->height()};
	for (const auto& [imageRect, personCount] : populationData) {
		r.drawSubImage(mIcons, position, imageRect);
		r.drawText(*FONT, std::to_string(personCount), position + fontOffset, NAS2D::Color::White);
		position.y() += 34;
	}
}
