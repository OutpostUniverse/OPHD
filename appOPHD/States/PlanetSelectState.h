#pragma once

#include <libControls/Button.h>
#include <libControls/TextArea.h>

#include <NAS2D/State.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Music.h>
#include <NAS2D/Resource/Sound.h>
#include <NAS2D/Renderer/Fade.h>

#include <vector>


namespace NAS2D
{
	enum class MouseButton;

	template <typename BaseType> struct Point;
}

struct PlanetAttributes;
class PlanetImage;


class PlanetSelectState : public NAS2D::State
{
public:
	PlanetSelectState();
	~PlanetSelectState() override;

protected:
	void initialize() override;
	State* update() override;

private:
	void onMouseDown(NAS2D::MouseButton, NAS2D::Point<int> position);

	void onMousePlanetEnter();
	void onMousePlanetExit();

	void onWindowResized(NAS2D::Vector<int> newSize);

	void onQuit();

private:
	const NAS2D::Font& mFontBold;
	const NAS2D::Font& mFontTiny;

	const NAS2D::Image mBackground;
	const NAS2D::Image mCloud1;
	const NAS2D::Image mCloud2;

	const NAS2D::Music mBackgroundMusic;

	const NAS2D::Sound mSelect;
	const NAS2D::Sound mHover;

	NAS2D::Timer mTimer;
	NAS2D::Fade mFade;

	NAS2D::State* mReturnState = this;

	Button mQuit;
	TextArea mPlanetDescription;

	std::size_t mPlanetSelection;
	std::vector<PlanetAttributes> mPlanetAttributes;
	std::vector<PlanetImage> mPlanets;
};
