#include "Slider.h"

#include "../../Cache.h"
#include "../../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <cmath>


using namespace NAS2D;


namespace {
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


/**
 * Get internal slider position.
 */
float Slider::positionInternal()
{
	return mPosition;
}


/**
 * set internal slider position
 */
void Slider::positionInternal(float newPosition)
{
	mPosition = std::clamp(newPosition, 0.0f, mLength);
	mSignal(mPosition);
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

	if (mSlider.contains(NAS2D::Point{x, y}))
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
	else if (mSlideBar.contains(NAS2D::Point{x, y}))
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

	if (mDisplayPosition)
	{
		mMouseHoverSlide = mSlideBar.contains(NAS2D::Point{x, y});
	}

	if (!mThumbPressed) { return; }

	if (mSliderType == SliderType::Vertical)
	{
		if (y < mSlideBar.y || y >(mSlideBar.y + mSlideBar.height))
		{
			return;
		}

		positionInternal(mLength * ((y - mSlideBar.y) / mSlideBar.height));
	}
	else
	{
		if (x < mSlideBar.x || x >(mSlideBar.x + mSlideBar.width))
		{
			return;
		}

		positionInternal(mLength * (x - mSlideBar.x) / mSlideBar.width);
	}
}


void Slider::logic()
{
	// compute position of items
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
	draw();
}


void Slider::draw()
{
	auto& renderer = Utility<Renderer>::get();

	mSkins.skinMiddle.draw(renderer, mSlideBar); // Slide area
	mSkins.skinButton1.draw(renderer, mButton1); // Top or left button
	mSkins.skinButton2.draw(renderer, mButton2); // Bottom or right button

	if (mSliderType == SliderType::Vertical)
	{
		// Fractional value can be dropped to avoid 'fuzzy' rendering due to texture filtering
		const auto i = static_cast<int>(mSlideBar.height / mLength);
		const auto newSize = std::max(i, mSlider.width);

		const auto relativeThumbPosition = static_cast<int>((mSlideBar.height - mSlider.height) * (mPosition / mLength)); //relative width

		mSlider = {mSlideBar.x, mSlideBar.y + relativeThumbPosition, mSlideBar.width, newSize};
	}
	else
	{
		// Fractional value can be dropped to avoid 'fuzzy' rendering due to texture filtering
		const auto i = static_cast<int>(mSlideBar.width / (mLength + 1.0f));
		const auto newSize = std::max(i, mSlider.height);

		const auto relativeThumbPosition = static_cast<int>((mSlideBar.width - mSlider.width) * (mPosition / mLength)); //relative width

		mSlider = {mSlideBar.x + relativeThumbPosition, mSlideBar.y, newSize, mSlideBar.height};
	}

	mSkins.skinSlider.draw(renderer, mSlider);

	if (mDisplayPosition && mMouseHoverSlide)
	{
		std::string textHover = std::to_string(static_cast<int>(thumbPosition())) + " / " + std::to_string(static_cast<int>(mLength));
		const auto boxSize = mFont.size(textHover) + NAS2D::Vector{4, 4};
		const auto boxPosition = (mSliderType == SliderType::Vertical) ?
			NAS2D::Point{mSlideBar.x + mSlideBar.width + 2, mMousePosition.y - boxSize.y} :
			NAS2D::Point{mMousePosition.x + 2, mSlideBar.y - 2 - boxSize.y};

		renderer.drawBox(NAS2D::Rectangle{boxPosition.x, boxPosition.y, boxSize.x, boxSize.y}, NAS2D::Color{255, 255, 255, 180});
		renderer.drawBoxFilled(NAS2D::Rectangle{boxPosition.x + 1, boxPosition.y + 1, boxSize.x - 2, boxSize.y - 2}, NAS2D::Color{0, 0, 0, 180});
		renderer.drawText(mFont, textHover, NAS2D::Point{boxPosition.x + 2, boxPosition.y + 2}, NAS2D::Color{220, 220, 220});
	}
}


/**
 * Set the current value
 */
void Slider::thumbPosition(float value)
{
	if (mBackward) { value = mLength - value; }

	mPosition = std::clamp(value, 0.0f, mLength);

	mSignal(thumbPosition());
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
	if (mBackward) { value = 1.0f - value; }
	mPosition = mLength * value;
	mSignal(thumbPosition());
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
