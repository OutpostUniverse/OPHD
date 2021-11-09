#include "Slider.h"

#include "../../Cache.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <cmath>


using namespace NAS2D;


namespace
{
	Slider::Skins loadSkins(Slider::SliderType sliderType)
	{
		if (sliderType == Slider::SliderType::Vertical)
		{
			return {
				{ // Button1
					imageCache.load("ui/skin/sv_bu_tl.png"),
					imageCache.load("ui/skin/sv_bu_tm.png"),
					imageCache.load("ui/skin/sv_bu_tr.png"),
					imageCache.load("ui/skin/sv_bu_ml.png"),
					imageCache.load("ui/skin/sv_bu_mm.png"),
					imageCache.load("ui/skin/sv_bu_mr.png"),
					imageCache.load("ui/skin/sv_bu_bl.png"),
					imageCache.load("ui/skin/sv_bu_bm.png"),
					imageCache.load("ui/skin/sv_bu_br.png")
				},
				{ // Middle
					imageCache.load("ui/skin/sv_sa_tl.png"),
					imageCache.load("ui/skin/sv_sa_tm.png"),
					imageCache.load("ui/skin/sv_sa_tr.png"),
					imageCache.load("ui/skin/sv_sa_ml.png"),
					imageCache.load("ui/skin/sv_sa_mm.png"),
					imageCache.load("ui/skin/sv_sa_mr.png"),
					imageCache.load("ui/skin/sv_sa_bl.png"),
					imageCache.load("ui/skin/sv_sa_bm.png"),
					imageCache.load("ui/skin/sv_sa_br.png")
				},
				{ // Button2
					imageCache.load("ui/skin/sv_bd_tl.png"),
					imageCache.load("ui/skin/sv_bd_tm.png"),
					imageCache.load("ui/skin/sv_bd_tr.png"),
					imageCache.load("ui/skin/sv_bd_ml.png"),
					imageCache.load("ui/skin/sv_bd_mm.png"),
					imageCache.load("ui/skin/sv_bd_mr.png"),
					imageCache.load("ui/skin/sv_bd_bl.png"),
					imageCache.load("ui/skin/sv_bd_bm.png"),
					imageCache.load("ui/skin/sv_bd_br.png")
				},
				{ // Slider
					imageCache.load("ui/skin/sv_sl_tl.png"),
					imageCache.load("ui/skin/sv_sl_tm.png"),
					imageCache.load("ui/skin/sv_sl_tr.png"),
					imageCache.load("ui/skin/sv_sl_ml.png"),
					imageCache.load("ui/skin/sv_sl_mm.png"),
					imageCache.load("ui/skin/sv_sl_mr.png"),
					imageCache.load("ui/skin/sv_sl_bl.png"),
					imageCache.load("ui/skin/sv_sl_bm.png"),
					imageCache.load("ui/skin/sv_sl_br.png")
				}
			};
		}
		else
		{
			return {
				{ // Button1
					imageCache.load("ui/skin/sh_bl_tl.png"),
					imageCache.load("ui/skin/sh_bl_tm.png"),
					imageCache.load("ui/skin/sh_bl_tr.png"),
					imageCache.load("ui/skin/sh_bl_ml.png"),
					imageCache.load("ui/skin/sh_bl_mm.png"),
					imageCache.load("ui/skin/sh_bl_mr.png"),
					imageCache.load("ui/skin/sh_bl_bl.png"),
					imageCache.load("ui/skin/sh_bl_bm.png"),
					imageCache.load("ui/skin/sh_bl_br.png")
				},
				{ // Middle
					imageCache.load("ui/skin/sh_sa_tl.png"),
					imageCache.load("ui/skin/sh_sa_tm.png"),
					imageCache.load("ui/skin/sh_sa_tr.png"),
					imageCache.load("ui/skin/sh_sa_ml.png"),
					imageCache.load("ui/skin/sh_sa_mm.png"),
					imageCache.load("ui/skin/sh_sa_mr.png"),
					imageCache.load("ui/skin/sh_sa_bl.png"),
					imageCache.load("ui/skin/sh_sa_bm.png"),
					imageCache.load("ui/skin/sh_sa_br.png")
				},
				{ // Button2
					imageCache.load("ui/skin/sh_br_tl.png"),
					imageCache.load("ui/skin/sh_br_tm.png"),
					imageCache.load("ui/skin/sh_br_tr.png"),
					imageCache.load("ui/skin/sh_br_ml.png"),
					imageCache.load("ui/skin/sh_br_mm.png"),
					imageCache.load("ui/skin/sh_br_mr.png"),
					imageCache.load("ui/skin/sh_br_bl.png"),
					imageCache.load("ui/skin/sh_br_bm.png"),
					imageCache.load("ui/skin/sh_br_br.png")
				},
				{ // Slider
					imageCache.load("ui/skin/sh_sl_tl.png"),
					imageCache.load("ui/skin/sh_sl_tm.png"),
					imageCache.load("ui/skin/sh_sl_tr.png"),
					imageCache.load("ui/skin/sh_sl_ml.png"),
					imageCache.load("ui/skin/sh_sl_mm.png"),
					imageCache.load("ui/skin/sh_sl_mr.png"),
					imageCache.load("ui/skin/sh_sl_bl.png"),
					imageCache.load("ui/skin/sh_sl_bm.png"),
					imageCache.load("ui/skin/sh_sl_br.png")
				}
			};
		}
	}
}


Slider::Slider(SliderType sliderType) : Slider(loadSkins(sliderType), sliderType)
{}


Slider::Slider(Slider::Skins skins, SliderType sliderType) :
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	mSliderType{sliderType},
	mSkins{skins}
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().connect(this, &Slider::onMouseDown);
	eventHandler.mouseButtonUp().connect(this, &Slider::onMouseUp);
	eventHandler.mouseMotion().connect(this, &Slider::onMouseMove);
}


Slider::~Slider()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect(this, &Slider::onMouseDown);
	eventHandler.mouseButtonUp().disconnect(this, &Slider::onMouseUp);
	eventHandler.mouseMotion().disconnect(this, &Slider::onMouseMove);
}


void Slider::buttonCheck(bool& buttonFlag, Rectangle<int>& rect, float value)
{
	if (rect.contains(mMousePosition))
	{
		changeThumbPosition(value);
		buttonFlag = true;

		mTimer.reset();
		mPressedAccumulator = 300;
		return;
	}
}


void Slider::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!enabled() || !visible()) { return; }

	if (button == EventHandler::MouseButton::Left)
	{
		if (mSlider.contains(NAS2D::Point{x, y}))
		{
			mThumbPressed = true;
			return;
		}

		buttonCheck(mButton1Held, mButton1, -1.0);
		buttonCheck(mButton2Held, mButton2, 1.0);
	}
}


void Slider::onMouseUp(EventHandler::MouseButton button, int x, int y)
{
	if (button != EventHandler::MouseButton::Left) { return; }

	mButton1Held = false;
	mButton2Held = false;
	mThumbPressed = false;

	if (!enabled() || !visible()) { return; }

	const auto mousePosition = NAS2D::Point{x, y};
	if (mSlideBar.contains(mousePosition) && !mSlider.contains(mousePosition))
	{
		if (mSliderType == SliderType::Vertical)
		{
			if (y < mSlider.y) { changeThumbPosition(-3.0); }
			else { changeThumbPosition(+3.0); }
		}
		else
		{
			if (x < mSlider.x) { changeThumbPosition(-3.0); }
			else { changeThumbPosition(+3.0); }
		}
	}
}


void Slider::onMouseMove(int x, int y, int /*dX*/, int /*dY*/)
{
	if (!enabled() || !visible()) { return; }

	mMousePosition = {x, y};

	if (!mThumbPressed) { return; }

	if (mSliderType == SliderType::Vertical)
	{
		if (y < mSlideBar.y || y >(mSlideBar.y + mSlideBar.height))
		{
			return;
		}

		thumbPosition(mLength * ((y - mSlideBar.y) / mSlideBar.height));
	}
	else
	{
		if (x < mSlideBar.x || x >(mSlideBar.x + mSlideBar.width))
		{
			return;
		}

		thumbPosition(mLength * (x - mSlideBar.x) / mSlideBar.width);
	}
}


void Slider::logic()
{
	if (mSliderType == SliderType::Vertical)
	{
		mButton1 = {mRect.x, mRect.y, mRect.width, mRect.width};
		mButton2 = {mRect.x, mRect.y + mRect.height - mRect.width, mRect.width, mRect.width};
		mSlideBar = {mRect.x, mRect.y + mRect.width, mRect.width, mRect.height - 2 * mRect.width};
	}
	else
	{
		mButton1 = {mRect.x, mRect.y, mRect.height, mRect.height};
		mButton2 = {mRect.x + mRect.width - mRect.height, mRect.y, mRect.height, mRect.height};
		mSlideBar = {mRect.x + mRect.height, mRect.y, mRect.width - 2 * mRect.height, mRect.height};
	}
}


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

	if (mSliderType == SliderType::Vertical)
	{
		// Fractional value can be dropped to avoid 'fuzzy' rendering due to texture filtering
		const auto i = static_cast<int>(mSlideBar.height / mLength);
		const auto newSize = std::max(i, mSlider.width);

		const auto relativeThumbPosition = static_cast<int>((mSlideBar.height - mSlider.height) * mPosition / mLength); //relative width

		mSlider = {mSlideBar.x, mSlideBar.y + relativeThumbPosition, mSlideBar.width, newSize};
	}
	else
	{
		// Fractional value can be dropped to avoid 'fuzzy' rendering due to texture filtering
		const auto i = static_cast<int>(mSlideBar.width / (mLength + 1.0f));
		const auto newSize = std::max(i, mSlider.height);

		const auto relativeThumbPosition = static_cast<int>((mSlideBar.width - mSlider.width) * mPosition / mLength); //relative width

		mSlider = {mSlideBar.x + relativeThumbPosition, mSlideBar.y, newSize, mSlideBar.height};
	}

	draw();
}


void Slider::draw() const
{
	auto& renderer = Utility<Renderer>::get();

	mSkins.skinMiddle.draw(renderer, mSlideBar); // Slide area
	mSkins.skinButton1.draw(renderer, mButton1); // Top or left button
	mSkins.skinButton2.draw(renderer, mButton2); // Bottom or right button
	mSkins.skinSlider.draw(renderer, mSlider);
}


void Slider::thumbPosition(float value)
{
	mPosition = std::clamp(value, 0.0f, mLength);
	mSignal(thumbPosition());
}


float Slider::thumbPosition() const
{
	return mPosition;
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
	thumbPosition(mPosition + change);
}


float Slider::length() const
{
	return mLength;
}


void Slider::length(float length)
{
	mLength = length;
	if (mPosition > mLength)
	{
		thumbPosition(mLength);
	}
}
