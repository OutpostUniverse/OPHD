#pragma once

#include "UI.h"

class PopulationPanel: public Control
{
public:
	PopulationPanel(): mIcons("ui/icons.png")
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
	}
	
	virtual ~PopulationPanel() {}

	void population(Population* pop) { mPopulation = pop; }
	void morale(int* m) { mMorale = m; }
	void old_morale(int* m) { mPreviousMorale = m; }

	void residential_capacity(int m) { mResidentialCapacity = m; }

	float capacity() const { return mCapacity; }

	virtual void update()
	{
		Renderer& r = Utility<Renderer>::get();
		r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkin);

		r.drawText(font(), string_format("Morale: %i", *mMorale), positionX() + 5, positionY() + 5, 255, 255, 255);
		r.drawText(font(), string_format("Previous: %i", *mPreviousMorale), positionX() + 5, positionY() + 15, 255, 255, 255);

		if (mResidentialCapacity == 0)
		{
			r.drawText(font(), string_format("Housing: %i / %i  (0%%)", mPopulation->size(), mResidentialCapacity), positionX() + 5, positionY() + 30, 255, 255, 255);
		}
		else
		{
			mCapacity = (static_cast<float>(mPopulation->size()) / static_cast<float>(mResidentialCapacity)) * 100.0f;
			r.drawText(font(), string_format("Housing: %i / %i  (%i%%)", mPopulation->size(), mResidentialCapacity, static_cast<int>(mCapacity)), positionX() + 5, positionY() + 30, 255, 255, 255);

		}

		r.drawSubImage(mIcons, positionX() + 5, positionY() + 45, 0, 96, 32, 32);		// Infant
		r.drawSubImage(mIcons, positionX() + 5, positionY() + 79, 32, 96, 32, 32);		// Student
		r.drawSubImage(mIcons, positionX() + 5, positionY() + 113, 64, 96, 32, 32);		// Worker
		r.drawSubImage(mIcons, positionX() + 5, positionY() + 147, 96, 96, 32, 32);		// Scientist
		r.drawSubImage(mIcons, positionX() + 5, positionY() + 181, 128, 96, 32, 32);	// Retired

		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_CHILD)), positionX() + 42, positionY() + 65, 255, 255, 255);
		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_STUDENT)), positionX() + 42, positionY() + 97, 255, 255, 255);
		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_WORKER)), positionX() + 42, positionY() + 129, 255, 255, 255);
		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_SCIENTIST)), positionX() + 42, positionY() + 160, 255, 255, 255);
		r.drawText(font(), string_format("%i", mPopulation->size(Population::ROLE_RETIRED)), positionX() + 42, positionY() + 193, 255, 255, 255);
	}

protected:

private:
	Image		mIcons;
	ImageList	mSkin;

	Population*	mPopulation = nullptr;

	int			mResidentialCapacity = 0;

	int*		mMorale = nullptr;
	int*		mPreviousMorale = nullptr;

	float		mCapacity = 0.0f;
};
