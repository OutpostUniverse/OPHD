// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "SplashState.h"

#include "MainMenuState.h"

#include <algorithm>

const int PAUSE_TIME = 5800;

unsigned int FADE_PAUSE_TIME = 5000;

const int FADE_LENGTH = 800;

float BYLINE_SCALE = 0.50f;
float BYLINE_SCALE_STEP = 0.000025f;
float BYLINE_ALPHA = -800.0f;
float BYLINE_ALPHA_FADE_STEP = 0.30f;

Timer BYLINE_TIMER;

enum LogoState
{
	LOGO_NONE,
	LOGO_LAIRWORKS,
	LOGO_NAS2D,
	LOGO_OUTPOSTHD
};

LogoState CURRENT_STATE = LOGO_NONE;


SplashState::SplashState() :	mLogoLairworks("sys/lairworks-logo.png"),
								mLogoNas2d("sys/nas2d_logo.png"),
								mLogoOutpostHd("sys/ophd_large.png"),
								mFlare("sys/flare_0.png"),
								mByline("sys/byline.png")
{}


SplashState::~SplashState()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.keyDown().disconnect(this, &SplashState::onKeyDown);
	e.mouseButtonDown().disconnect(this, &SplashState::onMouseDown);
}


void SplashState::initialize()
{
	EventHandler& e = Utility<EventHandler>::get();
	//e.keyDown().connect(this, &SplashState::onKeyDown);
	e.mouseButtonDown().connect(this, &SplashState::onMouseDown);

	Utility<Renderer>::get().showSystemPointer(false);
}


void setNextState(LogoState& _ls)
{
	if (_ls == LOGO_NONE)
	{
		_ls = LOGO_LAIRWORKS;
		FADE_PAUSE_TIME = 2500;
		return;
	}
	if (_ls == LOGO_LAIRWORKS)
	{
		_ls = LOGO_NAS2D;
		return;
	}
	if (_ls == LOGO_NAS2D)
	{
		_ls = LOGO_OUTPOSTHD;
		BYLINE_TIMER.reset();
		return;
	}
}


void SplashState::skipSplash()
{
	mReturnState = new MainMenuState();
	Utility<Renderer>::get().fadeOut((float)FADE_LENGTH);
}


/**
 * Code here is really crude but gets the job done. Figured it made more
 * sense to get something working that basically did what was needed than
 * something that was elegant.
 */
State* SplashState::update()
{
	Renderer& r = Utility<Renderer>::get();

	if (r.isFaded() && !r.isFading() && mTimer.accumulator() > FADE_PAUSE_TIME)
	{
		if (mReturnState != this) { return mReturnState; }

		setNextState(CURRENT_STATE);
		r.fadeIn((float)FADE_LENGTH);
		mTimer.reset();
	}

	if (CURRENT_STATE == LOGO_OUTPOSTHD) { r.drawBoxFilled(0, 0, r.width(), r.height(), 0, 0, 0); }
	else { r.drawBoxFilled(0, 0, r.width(), r.height(), 255, 255, 255); }


	if (CURRENT_STATE == LOGO_LAIRWORKS)
	{
		// Trunctation of fractional part of result is intentional
		// to prevent fuzzy images due to texture filtering
		int logoX = static_cast<int>(r.center_x() - mLogoLairworks.width() / 2);
		int logoY = static_cast<int>(r.center_y() - mLogoLairworks.height() / 2);
		r.drawImage(mLogoLairworks, static_cast<float>(logoX), static_cast<float>(logoY));
	}
	if (CURRENT_STATE == LOGO_NAS2D)
	{

		// Trunctation of fractional part of result is intentional
		// to prevent fuzzy images due to texture filtering
		int logoX = static_cast<int>(r.center_x() - mLogoNas2d.width() / 2);
		int logoY = static_cast<int>(r.center_y() - mLogoNas2d.height() / 2);
		r.drawImage(mLogoNas2d, static_cast<float>(logoX), static_cast<float>(logoY));
	}
	if (CURRENT_STATE == LOGO_OUTPOSTHD)
	{
		unsigned int tick = BYLINE_TIMER.delta();
		
		// Trunctation of fractional part of result is intentional
		// to prevent fuzzy images due to texture filtering
		int _x = static_cast<int>(r.center_x() - (mLogoOutpostHd.width() / 2)) - 100;
		int _y = static_cast<int>(r.center_y() - (mLogoOutpostHd.height() / 2));
		
		r.drawImageRotated(mFlare, static_cast<float>(_x) + 302 - 512, static_cast<float>(_y) + 241 - 512, BYLINE_TIMER.tick() / 600.0f);
		r.drawImage(mLogoOutpostHd, static_cast<float>(_x), static_cast<float>(_y));

		BYLINE_SCALE += tick * BYLINE_SCALE_STEP;
		BYLINE_ALPHA += tick * BYLINE_ALPHA_FADE_STEP;
		BYLINE_ALPHA = std::clamp(BYLINE_ALPHA, -3000.0f, 255.0f);

		if(BYLINE_ALPHA > 0.0f) { r.drawImage(mByline, r.center_x() - ((mByline.width() * BYLINE_SCALE) / 2), r.center_y() + 25, BYLINE_SCALE, 255, 255, 255, (int)BYLINE_ALPHA); }
	}
	
	if (r.isFading()) { return this; }

	if (CURRENT_STATE == LOGO_OUTPOSTHD)
	{
		if (mTimer.accumulator() > 11000)
		{
			skipSplash();
			return this;
		}
	}
	else if (mTimer.accumulator() > PAUSE_TIME)
	{
		r.fadeOut((float)FADE_LENGTH);
		mTimer.reset();
	}

	return mReturnState;
}


void SplashState::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat)
{
	skipSplash();
}


void SplashState::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	skipSplash();
}
