#include "SplashState.h"

#include "PlanetSelectState.h"


const int PAUSE_TIME = 5800;

const float FADE_LENGTH = 800.0f;

float LOGO_SCALE = 1.0f;

enum LogoState
{
	LOGO_NONE,
	LOGO_LAIRWORKS,
	LOGO_NAS2D,
	LOGO_OUTPOSTHD
};

LogoState CURRENT_STATE = LOGO_NONE;


SplashState::SplashState() :	mMousePointer("ui/pointers/normal.png"),
								mLogoLairworks("sys/lairworks-logo.png"),
								mLogoNas2d("sys/nas2d_logo.png"),
								mLogoOutpostHd("sys/ophd_large.png"),
								mSplash("music/splash.ogg")
{}


SplashState::~SplashState()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.keyDown().Disconnect(this, &SplashState::onKeyDown);
	e.mouseButtonDown().Disconnect(this, &SplashState::onMouseDown);
	e.mouseMotion().Disconnect(this, &SplashState::onMouseMove);
}


void SplashState::initialize()
{
	mReturnState = this;

	EventHandler& e = Utility<EventHandler>::get();

	e.keyDown().Connect(this, &SplashState::onKeyDown);

	e.mouseButtonDown().Connect(this, &SplashState::onMouseDown);
	e.mouseMotion().Connect(this, &SplashState::onMouseMove);

	Utility<Mixer>::get().fadeInMusic(mSplash, -1, 1000);
	Utility<Renderer>::get().fadeOut(0.0f);

	if (mLogoOutpostHd.width() > Utility<Renderer>::get().width())
	{
		LOGO_SCALE = Utility<Renderer>::get().width() / mLogoOutpostHd.width();
	}
}


void setNextState(LogoState& _ls)
{
	if (_ls == LOGO_NONE)
	{
		_ls = LOGO_LAIRWORKS;
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
		return;
	}
}


void SplashState::skipSplash()
{
	Utility<Mixer>::get().fadeOutMusic(FADE_LENGTH);
	Utility<Renderer>::get().fadeOut(FADE_LENGTH);
	mReturnState = new PlanetSelectState();
}

State* SplashState::update()
{
	Renderer& r = Utility<Renderer>::get();

	if (r.isFaded() && !r.isFading() && mTimer.accumulator() > 2500)
	{
		if (mReturnState != this)
			return mReturnState;

		setNextState(CURRENT_STATE);
		r.fadeIn(FADE_LENGTH);
		mTimer.reset();
	}

	if(CURRENT_STATE == LOGO_OUTPOSTHD)
		r.drawBoxFilled(0, 0, r.getScreenResolution().x(), r.getScreenResolution().y(), 0, 0, 0);
	else
		r.drawBoxFilled(0, 0, r.getScreenResolution().x(), r.getScreenResolution().y(), 255, 255, 255);

	if (CURRENT_STATE == LOGO_LAIRWORKS)
		r.drawImage(mLogoLairworks, r.screenCenterX() - mLogoLairworks.width() / 2, r.screenCenterY() - mLogoLairworks.height() / 2);
	if (CURRENT_STATE == LOGO_NAS2D)
		r.drawImage(mLogoNas2d, r.screenCenterX() - mLogoNas2d.width() / 2, r.screenCenterY() - mLogoNas2d.height() / 2);
	if (CURRENT_STATE == LOGO_OUTPOSTHD)
		r.drawImage(mLogoOutpostHd, r.screenCenterX() - ((mLogoOutpostHd.width() + 250 * LOGO_SCALE)) / 2, r.screenCenterY() - mLogoOutpostHd.height() / 2, LOGO_SCALE);


	r.drawImage(mMousePointer, mMousePosition.x(), mMousePosition.y());

	if (r.isFading())
		return this;

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
		r.fadeOut(FADE_LENGTH);
		mTimer.reset();
	}

	return mReturnState;
}


void SplashState::onKeyDown(KeyCode key, KeyModifier mod, bool repeat)
{
	skipSplash();
}


void SplashState::onMouseDown(MouseButton button, int x, int y)
{
	skipSplash();
}


void SplashState::onMouseMove(int x, int y, int rX, int rY)
{
	mMousePosition(x, y);
}





