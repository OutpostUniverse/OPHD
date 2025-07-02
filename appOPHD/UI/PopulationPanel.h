#pragma once

#include <libControls/Control.h>

#include <NAS2D/Renderer/RectangleSkin.h>

#include <vector>


struct MoraleChangeEntry;
class Population;
class PopulationPool;
class Morale;

namespace NAS2D
{
	class Font;
}


class PopulationPanel : public Control
{
public:
	PopulationPanel(const Population& pop, const PopulationPool& popPool, const Morale& morale);
	~PopulationPanel() override;

	void residentialCapacity(int val) { mResidentialCapacity = val; }

	void crimeRate(int val) { mCrimeRate = val; }
	int crimeRate() const { return mCrimeRate; }
	void draw() const override;

private:
	const NAS2D::Font& mFont;
	const NAS2D::Font& mFontBold;
	const NAS2D::Image& mIcons;
	NAS2D::RectangleSkin mSkin;

	std::vector<MoraleChangeEntry> mMoraleChangeReasons;

	const Population& mPopulation;
	const PopulationPool& mPopulationPool;
	const Morale& mMorale;

	int mResidentialCapacity{0};
	int mCrimeRate{0};
	int mPopulationPanelWidth{0};
};
