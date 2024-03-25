#pragma once

#include <libControls/Control.h>

#include <NAS2D/Renderer/RectangleSkin.h>

#include <string>
#include <vector>
#include <utility>


class Population;
class PopulationPool;

namespace NAS2D
{
	class Font;
}


class PopulationPanel : public Control
{
public:
	PopulationPanel(const Population& pop, const PopulationPool& popPool);

	void morale(int val) { mMorale = val; }
	void old_morale(int val) { mPreviousMorale = val; }

	void residentialCapacity(int val) { mResidentialCapacity = val; }

	void crimeRate(int val) { mCrimeRate = val; }
	int crimeRate() const { return mCrimeRate; }

	void addMoraleReason(const std::string& str, int val);

	const auto& moraleReasonList() const { return mMoraleChangeReasons; }

	void clearMoraleReasons() { mMoraleChangeReasons.clear(); }

	void update() override;

private:
	const NAS2D::Font& mFont;
	const NAS2D::Font& mFontBold;
	const NAS2D::Image& mIcons;
	NAS2D::RectangleSkin mSkin;

	std::vector<std::pair<std::string,int>> mMoraleChangeReasons;

	const Population& mPopulation;
	const PopulationPool& mPopulationPool;

	int mMorale{0};
	int mPreviousMorale{0};
	int mResidentialCapacity{0};
	int mCrimeRate{0};
	int mPopulationPanelWidth{0};
};
