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
	r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkin);

	r.drawText(*FONT, string_format("Morale: %i", *mMorale), positionX() + 5, positionY() + 5, 255, 255, 255);
	r.drawText(*FONT, string_format("Previous: %i", *mPreviousMorale), positionX() + 5, positionY() + 15, 255, 255, 255);

	mCapacity = (mResidentialCapacity > 0) ? (static_cast<float>(mPopulation->size()) / static_cast<float>(mResidentialCapacity)) * 100.0f : 0.0f;
	
	r.drawText(*FONT, string_format("Housing: %i / %i  (%i%%)", mPopulation->size(), mResidentialCapacity, static_cast<int>(mCapacity)), positionX() + 5, positionY() + 30, 255, 255, 255);

	r.drawSubImage(mIcons, positionX() + 5, positionY() + 45, 0, 96, 32, 32);		// Infant
	r.drawSubImage(mIcons, positionX() + 5, positionY() + 79, 32, 96, 32, 32);		// Student
	r.drawSubImage(mIcons, positionX() + 5, positionY() + 113, 64, 96, 32, 32);		// Worker
	r.drawSubImage(mIcons, positionX() + 5, positionY() + 147, 96, 96, 32, 32);		// Scientist
	r.drawSubImage(mIcons, positionX() + 5, positionY() + 181, 128, 96, 32, 32);	// Retired

	r.drawText(*FONT, std::to_string(mPopulation->size(Population::ROLE_CHILD)), positionX() + 42, positionY() + 65, 255, 255, 255);
	r.drawText(*FONT, std::to_string(mPopulation->size(Population::ROLE_STUDENT)), positionX() + 42, positionY() + 99, 255, 255, 255);
	r.drawText(*FONT, std::to_string(mPopulation->size(Population::ROLE_WORKER)), positionX() + 42, positionY() + 133, 255, 255, 255);
	r.drawText(*FONT, std::to_string(mPopulation->size(Population::ROLE_SCIENTIST)), positionX() + 42, positionY() + 167, 255, 255, 255);
	r.drawText(*FONT, std::to_string(mPopulation->size(Population::ROLE_RETIRED)), positionX() + 42, positionY() + 201, 255, 255, 255);
}
