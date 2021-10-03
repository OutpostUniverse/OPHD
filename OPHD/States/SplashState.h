#pragma once

#include "../Constants.h"

#include <NAS2D/State.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Image.h>


class SplashState : public NAS2D::State
{
public:
	SplashState();
	~SplashState() override;

protected:

	void initialize() override;
	NAS2D::State* update() override;

private:
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

	void skipSplash();

private:
	const NAS2D::Image mLogoLairworks;
	const NAS2D::Image mLogoNas2d;
	const NAS2D::Image mLogoOutpostHd;
	const NAS2D::Image mFlare;
	const NAS2D::Image mByline;

	NAS2D::Point<int> mMousePosition;

	NAS2D::Timer mTimer;

	NAS2D::State* mReturnState = this;
};
