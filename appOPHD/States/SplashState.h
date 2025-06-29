#pragma once

#include <NAS2D/State.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Renderer/Fade.h>

#include <cstdint>


namespace NAS2D
{
	enum class KeyModifier : uint16_t;
	enum class KeyCode : uint32_t;
	enum class MouseButton;
}


class SplashState : public NAS2D::State
{
public:
	SplashState();
	~SplashState() override;

protected:

	void initialize() override;
	NAS2D::State* update() override;

private:
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);

	void skipSplash();

private:
	const NAS2D::Image mLogoLairworks;
	const NAS2D::Image mLogoNas2d;
	const NAS2D::Image mLogoOutpostHd;
	const NAS2D::Image mFlare;
	const NAS2D::Image mByline;

	NAS2D::Point<int> mMousePosition;

	NAS2D::Timer mTimer;
	NAS2D::Fade mFade;

	NAS2D::State* mReturnState = this;
};
