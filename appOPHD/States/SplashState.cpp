#include "SplashState.h"

#include "MainMenuState.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Angle.h>

#include <algorithm>


namespace
{
	enum class LogoState
	{
		None,
		Lairworks,
		Nas2d,
		OutpostHD
	};


	LogoState currentState = LogoState::None;

	const int pauseTime = 5800;
	unsigned int fadePauseTime = 5000;
	const NAS2D::Duration fadeLength{800};

	NAS2D::Timer bylineTimer;


	LogoState setNextState(LogoState logoState)
	{
		if (logoState == LogoState::None)
		{
			fadePauseTime = 2500;
			return LogoState::Lairworks;
		}
		if (logoState == LogoState::Lairworks)
		{
			return LogoState::Nas2d;
		}

		bylineTimer.reset();
		return LogoState::OutpostHD;
	}
}


SplashState::SplashState() :
	mLogoLairworks("sys/lairworks-logo.png"),
	mLogoNas2d("sys/nas2d_logo.png"),
	mLogoOutpostHd("sys/ophd_large.png"),
	mFlare("sys/flare_0.png"),
	mByline("sys/byline.png")
{}


SplashState::~SplashState()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.keyDown().disconnect({this, &SplashState::onKeyDown});
	eventHandler.mouseButtonDown().disconnect({this, &SplashState::onMouseDown});
}


void SplashState::initialize()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.keyDown().connect({this, &SplashState::onKeyDown});
	eventHandler.mouseButtonDown().connect({this, &SplashState::onMouseDown});

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	renderer.showSystemPointer(false);
}


void SplashState::skipSplash()
{
	mReturnState = new MainMenuState();
	if (!mFade.isFading())
	{
		mFade.fadeOut(fadeLength);
	}
}


/**
 * Code here is really crude but gets the job done. Figured it made more
 * sense to get something working that basically did what was needed than
 * something that was elegant.
 */
NAS2D::State* SplashState::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	if (mFade.isFaded() && !mFade.isFading() && mTimer.elapsedTicks() > fadePauseTime)
	{
		if (mReturnState != this) { return mReturnState; }

		currentState = setNextState(currentState);
		mFade.fadeIn(fadeLength);
		mTimer.reset();
	}

	const auto size = renderer.size();
	const auto backgroundColor = (currentState == LogoState::OutpostHD) ? NAS2D::Color::Black : NAS2D::Color::White;
	renderer.drawBoxFilled(NAS2D::Rectangle{{0, 0}, size}, backgroundColor);


	if (currentState == LogoState::Lairworks)
	{
		renderer.drawImage(mLogoLairworks, renderer.center() - mLogoLairworks.size() / 2);
	}
	if (currentState == LogoState::Nas2d)
	{
		renderer.drawImage(mLogoNas2d, renderer.center() - mLogoNas2d.size() / 2);
	}
	if (currentState == LogoState::OutpostHD)
	{
		const unsigned int tick = bylineTimer.elapsedTicks();
		const auto logoPosition = renderer.center() - mLogoOutpostHd.size() / 2 - NAS2D::Vector{100, 0};

		renderer.drawImageRotated(mFlare, logoPosition + NAS2D::Vector{302 - 512, 241 - 512}, NAS2D::Angle::degrees(static_cast<float>(bylineTimer.tick()) / 600.0f));
		renderer.drawImage(mLogoOutpostHd, logoPosition);

		const float bylineScaleStep = 0.000025f;
		const float bylineAlphaFadeStep = 0.30f;
		const float bylineScale = 0.50f + static_cast<float>(tick) * bylineScaleStep;
		const float bylineAlpha = -800.0f + static_cast<float>(tick) * bylineAlphaFadeStep;
		const auto clampedBylineAlpha = static_cast<uint8_t>(std::clamp(bylineAlpha, 0.0f, 255.0f));

		if (clampedBylineAlpha > 0)
		{
			renderer.drawImage(mByline, renderer.center().to<float>() + NAS2D::Vector<float>{-mByline.size().x * bylineScale / 2, 25}, bylineScale, NAS2D::Color::White.alphaFade(clampedBylineAlpha));
		}
	}

	mFade.update();
	mFade.draw(renderer);

	if (mFade.isFaded()) { return this; }

	if (currentState == LogoState::OutpostHD)
	{
		if (mTimer.elapsedTicks() > 11000)
		{
			skipSplash();
			return this;
		}
	}
	else if (mTimer.elapsedTicks() > pauseTime)
	{
		mFade.fadeOut(fadeLength);
		mTimer.reset();
	}

	return mReturnState;
}


void SplashState::onKeyDown(NAS2D::KeyCode /*key*/, NAS2D::KeyModifier /*mod*/, bool /*repeat*/)
{
	skipSplash();
}


void SplashState::onMouseDown(NAS2D::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
{
	skipSplash();
}
