#pragma once

#include "UI.h"

class PopulationPanel: public Control
{
public:
	PopulationPanel(): mIcons("ui/icons.png"), mPopulation(nullptr), mMorale(nullptr)
	{
		size(125, 220);

		mSkin.push_back(Image("ui/skin/window_top_left.png"));
		mSkin.push_back(Image("ui/skin/window_top_middle.png"));
		mSkin.push_back(Image("ui/skin/window_top_right.png"));
		mSkin.push_back(Image("ui/skin/window_middle_left.png"));
		mSkin.push_back(Image("ui/skin/window_middle_middle.png"));
		mSkin.push_back(Image("ui/skin/window_middle_right.png"));
		mSkin.push_back(Image("ui/skin/window_bottom_left.png"));
		mSkin.push_back(Image("ui/skin/window_bottom_middle.png"));
		mSkin.push_back(Image("ui/skin/window_bottom_right.png"));
	}
	
	virtual ~PopulationPanel() {}

	void population(Population* pop) { mPopulation = pop; }
	void morale(int* m) { mMorale = m; }

	virtual void update()
	{
		Renderer& r = Utility<Renderer>::get();

		//r.drawBoxFilled(rect(), 0, 0, 0);
		r.drawImageRect(rect().x(), rect().y(), rect().w(), rect().h(), mSkin);

		r.drawText(font(), string_format("Morale: %i", *mMorale), positionX() + 5, positionY() + 5, 255, 255, 255);

		r.drawSubImage(mIcons, positionX() + 5, positionY() + 40, 0, 96, 32, 32);		// Infant
		r.drawSubImage(mIcons, positionX() + 5, positionY() + 74, 32, 96, 32, 32);		// Student
		r.drawSubImage(mIcons, positionX() + 5, positionY() + 108, 64, 96, 32, 32);		// Worker
		r.drawSubImage(mIcons, positionX() + 5, positionY() + 142, 96, 96, 32, 32);		// Scientist
		r.drawSubImage(mIcons, positionX() + 5, positionY() + 176, 128, 96, 32, 32);	// Retired

		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_CHILD)), positionX() + 42, positionY() + 60, 255, 255, 255);
		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_STUDENT)), positionX() + 42, positionY() + 92, 255, 255, 255);
		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_WORKER)), positionX() + 42, positionY() + 124, 255, 255, 255);
		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_SCIENTIST)), positionX() + 42, positionY() + 155, 255, 255, 255);
		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_RETIRED)), positionX() + 42, positionY() + 188, 255, 255, 255);
	}

protected:

private:
	Image		mIcons;

	ImageList	mSkin;

	Population*	mPopulation;
	int*		mMorale;
};