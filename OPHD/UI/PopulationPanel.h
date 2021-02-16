#pragma once

#include "Core/Control.h"

#include <NAS2D/Resources/Font.h>
#include <NAS2D/Renderer/RectangleSkin.h>


class Population;


class PopulationPanel: public Control
{
public:
	PopulationPanel();

	void population(Population* pop) { mPopulation = pop; }

	void morale(int val) { mMorale = val; }
	void old_morale(int val) { mPreviousMorale = val; }

	void residential_capacity(int m) { mResidentialCapacity = m; }

	/**
	 * \fixme	This class/function is use to store residence capacity
	 *			by the MapViewState. Probably not an appropriate place.
	 */
	int capacity() const { return mCapacity; }

	void update() override;

private:
	const NAS2D::Font& mFont;
	const NAS2D::Font& mFontBold;
	const NAS2D::Image& mIcons;
	NAS2D::RectangleSkin mSkin;

	Population* mPopulation = nullptr;

	int mResidentialCapacity = 0;

	int mMorale{ 0 };
	int mPreviousMorale{ 0 };

	int mCapacity{ 0 };
};
