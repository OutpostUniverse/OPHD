// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
 * \file	Slider.c++
 * \brief	Functions for sliding controls.
 * \author	Goof
 */
#include "Slider.h"

#include "../../Constants.h"
#include "../../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"

#include <algorithm>

using namespace NAS2D;


static Font* SLD_FONT = nullptr;

/**
 * C'tor
 */
Slider::Slider() :	Control()
{
	SLD_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &Slider::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().connect(this, &Slider::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().connect(this, &Slider::onMouseMotion);
	hasFocus(true);
}


/**
 * D'tor
 */
Slider::~Slider()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &Slider::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().disconnect(this, &Slider::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &Slider::onMouseMotion);
}


/**
 *
 */
void Slider::size(float w, float h)
{
	Control::size(w, h);

	// deduce the type of slider from the ratio.
	if (rect().height() > rect().width())
	{
		mSliderType = SLIDER_VERTICAL;
	}
	else
	{
		mSliderType = SLIDER_HORIZONTAL;
	}
}


/**
 *
 */
void Slider::setSkins()
{
	if (!mSkinButton1.empty()) { return; }

	if (mSliderType == SLIDER_VERTICAL)
	{
		mSkinButton1.push_back(Image("ui/skin/sv_bu_tl.png"));
		mSkinButton1.push_back(Image("ui/skin/sv_bu_tm.png"));
		mSkinButton1.push_back(Image("ui/skin/sv_bu_tr.png"));
		mSkinButton1.push_back(Image("ui/skin/sv_bu_ml.png"));
		mSkinButton1.push_back(Image("ui/skin/sv_bu_mm.png"));
		mSkinButton1.push_back(Image("ui/skin/sv_bu_mr.png"));
		mSkinButton1.push_back(Image("ui/skin/sv_bu_bl.png"));
		mSkinButton1.push_back(Image("ui/skin/sv_bu_bm.png"));
		mSkinButton1.push_back(Image("ui/skin/sv_bu_br.png"));

		mSkinMiddle.push_back(Image("ui/skin/sv_sa_tl.png"));
		mSkinMiddle.push_back(Image("ui/skin/sv_sa_tm.png"));
		mSkinMiddle.push_back(Image("ui/skin/sv_sa_tr.png"));
		mSkinMiddle.push_back(Image("ui/skin/sv_sa_ml.png"));
		mSkinMiddle.push_back(Image("ui/skin/sv_sa_mm.png"));
		mSkinMiddle.push_back(Image("ui/skin/sv_sa_mr.png"));
		mSkinMiddle.push_back(Image("ui/skin/sv_sa_bl.png"));
		mSkinMiddle.push_back(Image("ui/skin/sv_sa_bm.png"));
		mSkinMiddle.push_back(Image("ui/skin/sv_sa_br.png"));

		mSkinButton2.push_back(Image("ui/skin/sv_bd_tl.png"));
		mSkinButton2.push_back(Image("ui/skin/sv_bd_tm.png"));
		mSkinButton2.push_back(Image("ui/skin/sv_bd_tr.png"));
		mSkinButton2.push_back(Image("ui/skin/sv_bd_ml.png"));
		mSkinButton2.push_back(Image("ui/skin/sv_bd_mm.png"));
		mSkinButton2.push_back(Image("ui/skin/sv_bd_mr.png"));
		mSkinButton2.push_back(Image("ui/skin/sv_bd_bl.png"));
		mSkinButton2.push_back(Image("ui/skin/sv_bd_bm.png"));
		mSkinButton2.push_back(Image("ui/skin/sv_bd_br.png"));

		mSkinSlider.push_back(Image("ui/skin/sv_sl_tl.png"));
		mSkinSlider.push_back(Image("ui/skin/sv_sl_tm.png"));
		mSkinSlider.push_back(Image("ui/skin/sv_sl_tr.png"));
		mSkinSlider.push_back(Image("ui/skin/sv_sl_ml.png"));
		mSkinSlider.push_back(Image("ui/skin/sv_sl_mm.png"));
		mSkinSlider.push_back(Image("ui/skin/sv_sl_mr.png"));
		mSkinSlider.push_back(Image("ui/skin/sv_sl_bl.png"));
		mSkinSlider.push_back(Image("ui/skin/sv_sl_bm.png"));
		mSkinSlider.push_back(Image("ui/skin/sv_sl_br.png"));
	}
	else
	{
		mSkinButton1.push_back(Image("ui/skin/sh_bl_tl.png"));
		mSkinButton1.push_back(Image("ui/skin/sh_bl_tm.png"));
		mSkinButton1.push_back(Image("ui/skin/sh_bl_tr.png"));
		mSkinButton1.push_back(Image("ui/skin/sh_bl_ml.png"));
		mSkinButton1.push_back(Image("ui/skin/sh_bl_mm.png"));
		mSkinButton1.push_back(Image("ui/skin/sh_bl_mr.png"));
		mSkinButton1.push_back(Image("ui/skin/sh_bl_bl.png"));
		mSkinButton1.push_back(Image("ui/skin/sh_bl_bm.png"));
		mSkinButton1.push_back(Image("ui/skin/sh_bl_br.png"));

		mSkinMiddle.push_back(Image("ui/skin/sh_sa_tl.png"));
		mSkinMiddle.push_back(Image("ui/skin/sh_sa_tm.png"));
		mSkinMiddle.push_back(Image("ui/skin/sh_sa_tr.png"));
		mSkinMiddle.push_back(Image("ui/skin/sh_sa_ml.png"));
		mSkinMiddle.push_back(Image("ui/skin/sh_sa_mm.png"));
		mSkinMiddle.push_back(Image("ui/skin/sh_sa_mr.png"));
		mSkinMiddle.push_back(Image("ui/skin/sh_sa_bl.png"));
		mSkinMiddle.push_back(Image("ui/skin/sh_sa_bm.png"));
		mSkinMiddle.push_back(Image("ui/skin/sh_sa_br.png"));

		mSkinButton2.push_back(Image("ui/skin/sh_br_tl.png"));
		mSkinButton2.push_back(Image("ui/skin/sh_br_tm.png"));
		mSkinButton2.push_back(Image("ui/skin/sh_br_tr.png"));
		mSkinButton2.push_back(Image("ui/skin/sh_br_ml.png"));
		mSkinButton2.push_back(Image("ui/skin/sh_br_mm.png"));
		mSkinButton2.push_back(Image("ui/skin/sh_br_mr.png"));
		mSkinButton2.push_back(Image("ui/skin/sh_br_bl.png"));
		mSkinButton2.push_back(Image("ui/skin/sh_br_bm.png"));
		mSkinButton2.push_back(Image("ui/skin/sh_br_br.png"));

		mSkinSlider.push_back(Image("ui/skin/sh_sl_tl.png"));
		mSkinSlider.push_back(Image("ui/skin/sh_sl_tm.png"));
		mSkinSlider.push_back(Image("ui/skin/sh_sl_tr.png"));
		mSkinSlider.push_back(Image("ui/skin/sh_sl_ml.png"));
		mSkinSlider.push_back(Image("ui/skin/sh_sl_mm.png"));
		mSkinSlider.push_back(Image("ui/skin/sh_sl_mr.png"));
		mSkinSlider.push_back(Image("ui/skin/sh_sl_bl.png"));
		mSkinSlider.push_back(Image("ui/skin/sh_sl_bm.png"));
		mSkinSlider.push_back(Image("ui/skin/sh_sl_br.png"));
	}
}


/*!
 * Get internal slider position.
 */
float Slider::positionInternal()
{
	return mPosition;
}


/**
 *  \brief set internal slider position
 */
void Slider::positionInternal(float newPosition)
{
	mPosition = std::clamp(newPosition, 0.0f, mLength);
	mCallback(mPosition);
}


void Slider::_buttonCheck(bool& buttonFlag, Rectangle_2df& rect, float value)
{
	if (rect.to<int>().contains(mMousePosition))
	{
		changeThumbPosition(value);
		buttonFlag = true;

		mTimer.reset();
		mPressedAccumulator = 300;
		return;
	}
}


/**
 *
 */
void Slider::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!enabled() || !visible() || !hasFocus()) { return; }

	if (button == EventHandler::MouseButton::BUTTON_LEFT)
	{
		if (mSlider.to<int>().contains(NAS2D::Point{x, y}))
		{
			mThumbPressed = true;
			return;
		}

		_buttonCheck(mButton1Held, mButton1, -1.0);
		_buttonCheck(mButton2Held, mButton2, 1.0);
	}
}


/**
 *
 */
void Slider::onMouseUp(EventHandler::MouseButton button, int x, int y)
{
	if (button != EventHandler::MouseButton::BUTTON_LEFT) { return; }
	
	mButton1Held = false;
	mButton2Held = false;
	mThumbPressed = false;

	if (!enabled() || !visible() || !hasFocus()) { return; }

	if (mSlider.to<int>().contains(NAS2D::Point{x, y}))
	{
		// nothing
	}
	/*
	else if (pointInRect_f(x, y, mButton2))
	{
		changeThumbPosition(+1.0);
	}
	else if (pointInRect_f(x, y, mButton1))
	{
		changeThumbPosition(-1.0);
	}
	*/
	else if (mSlideBar.to<int>().contains(NAS2D::Point{x, y}))
	{
		if (mSliderType == SLIDER_VERTICAL)
		{
			if (y < mSlider.y()) { changeThumbPosition(-3.0); }
			else { changeThumbPosition(+3.0); }
		}
		else
		{
			if (x < mSlider.x()) { changeThumbPosition(-3.0); }
			else { changeThumbPosition(+3.0); }
		}
	}
}


/**
 *
 */
void Slider::onMouseMotion(int x, int y, int /*dX*/, int /*dY*/)
{
	if (!enabled() || !visible() || !hasFocus()) { return; }

	mMousePosition = {x, y};

	if (mDisplayPosition)
	{
		mMouseHoverSlide = mSlideBar.to<int>().contains(NAS2D::Point{x, y});
	}

	if (!mThumbPressed) { return; }

	if (mSliderType == SLIDER_VERTICAL)
	{
		if (y < mSlideBar.y() || y >(mSlideBar.y() + mSlideBar.height()))
		{
			return;
		}

		positionInternal(mLength * ((y - mSlideBar.y()) / mSlideBar.height()));
	}
	else
	{
		if (x < mSlideBar.x() || x >(mSlideBar.x() + mSlideBar.width()))
		{
			return;
		}

		positionInternal(mLength * (x - mSlideBar.x()) / mSlideBar.width());
	}
}


/**
 *
 */
void Slider::logic()
{
	// compute position of items
	if (mSliderType == SLIDER_VERTICAL)
	{
		mButton1.x(rect().x());
		mButton1.y(rect().y());
		mButton1.width(rect().width());
		mButton1.height(rect().width());

		mButton2.x(rect().x());
		mButton2.y(rect().y() + rect().height() - rect().width());
		mButton2.width(rect().width());
		mButton2.height(rect().width());

		mSlideBar.x(rect().x());
		mSlideBar.y(rect().y() + rect().width());
		mSlideBar.width(rect().width());
		mSlideBar.height(rect().height() - 2 * rect().width());
	}
	else
	{
		mButton1.x(rect().x());
		mButton1.y(rect().y());
		mButton1.width(rect().height());
		mButton1.height(rect().height());

		mButton2.x(rect().x() + rect().width() - rect().height());
		mButton2.y(rect().y());
		mButton2.width(rect().height());
		mButton2.height(rect().height());

		mSlideBar.x(rect().x() + rect().height());
		mSlideBar.y(rect().y());
		mSlideBar.width(rect().width() - 2 * rect().height());
		mSlideBar.height(rect().height());
	}
}


/**
 *
 */
void Slider::update()
{
	if (!visible()) { return; }

	if (mButton1Held || mButton2Held)
	{
		if (mTimer.accumulator() >= mPressedAccumulator)
		{
			mPressedAccumulator = 30;
			mTimer.reset();
			if (mButton1Held) { changeThumbPosition(-1.0); }
			else { changeThumbPosition(1.0); }
		}
	}

	logic();
	setSkins();
	draw();
}


/**
 *
 */
void Slider::draw()
{
	Renderer& r = Utility<Renderer>::get();
	std::string textHover;
	int _x = 0, _y = 0, _w = 0, _h = 0;
	float _thumbPosition = 0.0f;

	if (mSliderType == SLIDER_VERTICAL)
	{
		r.drawImageRect(mSlideBar.x(), mSlideBar.y(), mSlideBar.width(), mSlideBar.height(), mSkinMiddle);// slide area
		r.drawImageRect(mButton1.x(), mButton1.y(), mButton1.height(), mButton1.height(), mSkinButton1);// top button
		r.drawImageRect(mButton2.x(), mButton2.y(), mButton2.height(), mButton2.height(), mSkinButton2);// bottom button
		//r.drawImageRect(mButtonUp.x(), mButtonUp.y(), mButtonUp.height(), mButtonUp.height(), mSkinButtonLeft);// top button

		// Slider
		mSlider.width(mSlideBar.width()); // height = slide bar height
		
		// Fractional value can be dropped to avoid 'fuzzy' rendering due to texture filtering
		int height_i = static_cast<int>(mSlideBar.height() / mLength);
		mSlider.height(static_cast<float>(height_i)); //relative width
		if (mSlider.height() < mSlider.width()) // not too relative. Minimum = Heigt itself
		{
			mSlider.height(mSlider.width());
		}

		_thumbPosition = (mSlideBar.height() - mSlider.height())  * (mPosition / mLength); //relative width

		mSlider.x(mSlideBar.x());
		mSlider.y(mSlideBar.y() + _thumbPosition);
		r.drawImageRect(mSlider.x(), mSlider.y(), mSlider.width(), mSlider.height(), mSkinSlider);
	}
	else
	{
		r.drawImageRect(mSlideBar.x(), mSlideBar.y(), mSlideBar.width(), mSlideBar.height(), mSkinMiddle);	// slide area
		r.drawImageRect(mButton1.x(), mButton1.y(), mButton1.height(), mButton1.height(), mSkinButton1);	// left button
		r.drawImageRect(mButton2.x(), mButton2.y(), mButton2.height(), mButton2.height(), mSkinButton2);	// right button

		// Slider
		mSlider.height(mSlideBar.height());	// height = slide bar height

		// Fractional value can be dropped to avoid 'fuzzy' rendering due to texture filtering
		int width_i = static_cast<int>(mSlideBar.width() / (mLength + 1.0f));
		mSlider.width(static_cast<float>(width_i)); //relative width
		if (mSlider.width() < mSlider.height())	// not too relative. Minimum = Heigt itself
		{
			mSlider.width(mSlider.height());
		}

		_thumbPosition = (mSlideBar.width() - mSlider.width())  * (mPosition / mLength); //relative width

		mSlider.x(mSlideBar.x() + _thumbPosition);
		mSlider.y(mSlideBar.y());
		r.drawImageRect(mSlider.x(), mSlider.y(), mSlider.width(), mSlider.height(), mSkinSlider);
	}

	if (mDisplayPosition && mMouseHoverSlide)
	{
		textHover = string_format("%i / %i", static_cast<int>(thumbPosition()), static_cast<int>(mLength));
		_w = SLD_FONT->width(textHover) + 4;
		_h = SLD_FONT->height() + 4;

		if (mSliderType == SLIDER_VERTICAL)
		{
			_x = static_cast<int>(mSlideBar.x() + mSlideBar.width() + 2.0f);
			_y = mMousePosition.y() - _h;
		}
		else
		{
			_x = mMousePosition.x() + 2;
			_y = static_cast<int>(mSlideBar.y() - 2.0f) - _h;
		}

		r.drawBox(static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_w), static_cast<float>(_h), 255, 255, 255, 180);
		r.drawBoxFilled(static_cast<float>(_x) + 1.0f, static_cast<float>(_y) + 1.0f, static_cast<float>(_w) - 2.0f, static_cast<float>(_h) - 2.0f, 0, 0, 0, 180);
		r.drawText(*SLD_FONT, textHover, static_cast<float>(_x) + 2.0f, static_cast<float>(_y) + 2.0f, 220, 220, 220);
	}
}


/**
 * Set the current value
 */
void Slider::thumbPosition(float value)
{
	if (mBackward) { value = mLength - value; }

	mPosition = value;

	if (mPosition < 0.0)
	{
		mPosition = 0.0;
	}
	else if (mPosition > mLength)
	{
		mPosition = mLength;
	}

	mCallback(thumbPosition());
}


/**
* Gets the current value of position
*/
float Slider::thumbPosition()
{
	float value = mPosition;
	if (mBackward)
	{
		value = mLength - value;
	}

	return value;
}


/**
 * Adds the change amount to the current position.
 *
 * \param	change	Amount to change in percent to change the
 *					slider's position. Must be between 0.0
 *					1.0.
 */
void Slider::changeThumbPosition(float change)
{
	positionInternal(mPosition + change);
}


void Slider::thumbPositionNormalized(float value) {
	value = std::clamp(value, 0.0f, 1.0f);
	if(mBackward) { value = 1.0f - value; }
	mPosition = mLength * value;
	mCallback(thumbPosition());
}

float Slider::thumbPositionNormalized() {
	return mPosition / mLength;
}

/**
 * Returns the max value position can get
 */
float Slider::length()
{
	return mLength;
}


/**
 * Set the max value position can get
 */
void Slider::length(float length)
{
	mLength = length;
	if (mPosition > mLength)
	{
		thumbPosition(mLength);
	}
}
