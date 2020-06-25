// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "SplashState.h"

#include "MainMenuState.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"

#include <algorithm>

const int PAUSE_TIME = 5800;

unsigned int FADE_PAUSE_TIME = 5000;

const float FADE_LENGTH = 800;

float BYLINE_SCALE = 0.50f;
float BYLINE_SCALE_STEP = 0.000025f;
float BYLINE_ALPHA = -800.0f;
float BYLINE_ALPHA_FADE_STEP = 0.30f;

NAS2D::Timer BYLINE_TIMER;

enum LogoState
{
	LOGO_NONE,
	LOGO_LAIRWORKS,
	LOGO_NAS2D,
	LOGO_OUTPOSTHD
};

LogoState CURRENT_STATE = LogoState::LOGO_NONE;


SplashState::SplashState() :
	mLogoLairworks("sys/lairworks-logo.png"),
	mLogoNas2d("sys/nas2d_logo.png"),
	mLogoOutpostHd("sys/ophd_large.png"),
	mFlare("sys/flare_0.png"),
	mByline("sys/byline.png")
{}


SplashState::~SplashState()
{
	NAS2D::EventHandler& e = NAS2D::Utility<NAS2D::EventHandler>::get();
	e.keyDown().disconnect(this, &SplashState::onKeyDown);
	e.mouseButtonDown().disconnect(this, &SplashState::onMouseDown);
}


void SplashState::initialize()
{
	NAS2D::EventHandler& e = NAS2D::Utility<NAS2D::EventHandler>::get();
	//e.keyDown().connect(this, &SplashState::onKeyDown);
	e.mouseButtonDown().connect(this, &SplashState::onMouseDown);

	NAS2D::Utility<NAS2D::Renderer>::get().showSystemPointer(false);
}


void setNextState(LogoState& _ls)
{
	if (_ls == LogoState::LOGO_NONE)
	{
		_ls = LogoState::LOGO_LAIRWORKS;
		FADE_PAUSE_TIME = 2500;
		return;
	}
	if (_ls == LogoState::LOGO_LAIRWORKS)
	{
		_ls = LogoState::LOGO_NAS2D;
		return;
	}
	if (_ls == LogoState::LOGO_NAS2D)
	{
		_ls = LogoState::LOGO_OUTPOSTHD;
		BYLINE_TIMER.reset();
		return;
	}
}


void SplashState::skipSplash()
{
	mReturnState = new MainMenuState();
	NAS2D::Utility<NAS2D::Renderer>::get().fadeOut(FADE_LENGTH);
}


/**
 * Code here is really crude but gets the job done. Figured it made more
 * sense to get something working that basically did what was needed than
 * something that was elegant.
 */
NAS2D::State* SplashState::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	if (renderer.isFaded() && !renderer.isFading() && mTimer.accumulator() > FADE_PAUSE_TIME)
	{
		if (mReturnState != this) { return mReturnState; }

		setNextState(CURRENT_STATE);
		renderer.fadeIn(FADE_LENGTH);
		mTimer.reset();
	}

	const auto size = renderer.size();
	const auto backgroundColor = (CURRENT_STATE == LogoState::LOGO_OUTPOSTHD) ? NAS2D::Color::Black : NAS2D::Color::White;
	renderer.drawBoxFilled({0, 0, size.x, size.y}, backgroundColor);


	if (CURRENT_STATE == LogoState::LOGO_LAIRWORKS)
	{
		renderer.drawImage(mLogoLairworks, renderer.center() - mLogoLairworks.size() / 2);
	}
	if (CURRENT_STATE == LogoState::LOGO_NAS2D)
	{
		renderer.drawImage(mLogoNas2d, renderer.center() - mLogoNas2d.size() / 2);
	}
	if (CURRENT_STATE == LogoState::LOGO_OUTPOSTHD)
	{
		const unsigned int tick = BYLINE_TIMER.delta();
		const auto logoPosition = renderer.center() - mLogoOutpostHd.size() / 2 - NAS2D::Vector{100, 0};

		renderer.drawImageRotated(mFlare, logoPosition + NAS2D::Vector{302 - 512, 241 - 512}, BYLINE_TIMER.tick() / 600.0f);
		renderer.drawImage(mLogoOutpostHd, logoPosition);

		BYLINE_SCALE += tick * BYLINE_SCALE_STEP;
		BYLINE_ALPHA += tick * BYLINE_ALPHA_FADE_STEP;
		BYLINE_ALPHA = std::clamp(BYLINE_ALPHA, -3000.0f, 255.0f);

		if(BYLINE_ALPHA > 0.0f) { renderer.drawImage(mByline, renderer.center_x() - ((mByline.width() * BYLINE_SCALE) / 2), renderer.center_y() + 25, BYLINE_SCALE, 255, 255, 255, static_cast<uint8_t>(BYLINE_ALPHA)); }
	}
	
	if (renderer.isFading()) { return this; }

	if (CURRENT_STATE == LogoState::LOGO_OUTPOSTHD)
	{
		if (mTimer.accumulator() > 11000)
		{
			skipSplash();
			return this;
		}
	}
	else if (mTimer.accumulator() > PAUSE_TIME)
	{
		renderer.fadeOut(FADE_LENGTH);
		mTimer.reset();
	}

	return mReturnState;
}


void SplashState::onKeyDown(NAS2D::EventHandler::KeyCode /*key*/, NAS2D::EventHandler::KeyModifier /*mod*/, bool /*repeat*/)
{
	skipSplash();
}


void SplashState::onMouseDown(NAS2D::EventHandler::MouseButton /*button*/, int /*x*/, int /*y*/)
{
	skipSplash();
}
