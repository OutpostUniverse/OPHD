#include "Slider.h"



/*!
 * \file	Slider.c++
 * \brief	Functions for sliding controls.
 * \author	Goof
 */

Slider::Slider() :  Control(),
					mMouseX(0), mMouseY(0),
					mPosition(0.0),
					mThumbPressed(false),
					mMouseHover(0),
					mDisplayPosition(0),
					mBackward(false)
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &Slider::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().connect(this, &Slider::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().connect(this, &Slider::onMouseMotion);
	hasFocus(true);

	
}

Slider::~Slider(){
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &Slider::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().disconnect(this, &Slider::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &Slider::onMouseMotion);
}

void Slider::size(float w, float h) 
{
	Control::size(w,h);

	// deduce the type of slider from the ratio.
	if (rect().height() > rect().width())
	{
		mSliderType = SLIDER_VERTICAL;
	} else {
		mSliderType = SLIDER_HORIZONTAL;
	}
}
void Slider::setSkins() {
	if (!mSkinButton1.empty())
		return;
	if (mSliderType == SLIDER_VERTICAL) {
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
	} else {
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

void Slider::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!enabled() || !visible() || !hasFocus())
		return;

	if (button == EventHandler::BUTTON_LEFT)
	{
		if (pointInRect_f(x,y, mSlider)) {
			mThumbPressed = true;
		}
	}
}


void Slider::onMouseUp(EventHandler::MouseButton button, int x, int y)
{
	if (button != EventHandler::BUTTON_LEFT)
		return;
	mThumbPressed = false;

	if (!enabled() || !visible() || !hasFocus())
		return;

	if (pointInRect_f(x, y, mSlider)) {
		// nothing
	}else if (pointInRect_f(x, y, mButton2)) {
		changePosition(+1.0);
	}else if (pointInRect_f(x, y, mButton1)){
		changePosition(-1.0);
	}else if(pointInRect_f(x, y, mSlideBar)){

		if (mSliderType == SLIDER_VERTICAL)
		{
			if (y<mSlider.y())
				changePosition(-3.0);
			else
				changePosition(+3.0);
		}else{
			if(x<mSlider.x())
				changePosition(-3.0);
			else
				changePosition(+3.0);
		}
	}
}


void Slider::onMouseMotion(int x, int y, int dX, int dY)
{
	if (!enabled() || !visible() || !hasFocus())
		return;

	if(mDisplayPosition){
		if (mSliderType == SLIDER_VERTICAL)
			mMouseHoverSlide = pointInRect_f(x, y, mSlideBar);
		else
			mMouseHoverSlide = pointInRect_f(x, y, mSlideBar);
	}

	if (!pointInRect_f(x, y, rect()))
		return;

	mMouseX = x;
	mMouseY = y;

	if(!mThumbPressed)
		return;

	if (mSliderType == SLIDER_VERTICAL)
		mMouseHover = pointInRect_f(x, y, mSlideBar.x() - mSlideBar.width(), mSlideBar.y(), mSlideBar.width() * 3, mSlideBar.height());
	else
		mMouseHover = pointInRect_f(x, y, mSlideBar.x(), mSlideBar.y() - mSlideBar.height(), mSlideBar.width(), mSlideBar.height() * 3);

	if (!mMouseHover)
		return;

	if (mSliderType == SLIDER_VERTICAL)
	{
		if (y < mSlideBar.y())
			return;
		if (y >(mSlideBar.y() + mSlideBar.height()))
			return;
		positionInternal(mLenght * ((y - mSlideBar.y()) / mSlideBar.height()));
	} else {
		if (x < mSlideBar.x())
			return;
		if (x > (mSlideBar.x()+mSlideBar.width()))
			return;
		positionInternal(mLenght * (x - mSlideBar.x()) / mSlideBar.width());
		
	}

}


void Slider::logic()
{
	// compute position of items
	if(mSliderType == SLIDER_VERTICAL)
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
	} else {
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

void Slider::update()
{
	logic();
	setSkins();
	draw();
}

void Slider::draw()
{
	if (!visible())
		return;

	Renderer& r = Utility<Renderer>::get();
	string textHover;
	int _x, _y, _w, _h;
	float thumbPosition;

	if (mSliderType == SLIDER_VERTICAL)
	{

		r.drawImageRect(mSlideBar.x(), mSlideBar.y(), mSlideBar.width(), mSlideBar.height(), mSkinMiddle);// slide area
		r.drawImageRect(mButton1.x(), mButton1.y(), mButton1.height(), mButton1.height(), mSkinButton1);// top button
		r.drawImageRect(mButton2.x(), mButton2.y(), mButton2.height(), mButton2.height(), mSkinButton2);// bottom button																											   //r.drawImageRect(mButtonUp.x(), mButtonUp.y(), mButtonUp.height(), mButtonUp.height(), mSkinButtonLeft);// top button

		// Slider
		mSlider.width(mSlideBar.width());	// height = slide bar height
		mSlider.height(static_cast<int>(mSlideBar.height() / mLenght)); //relative width 
		if (mSlider.height() < mSlider.width())	// not too relative. Minimum = Heigt itself
			mSlider.height(mSlider.width());
		thumbPosition = (mSlideBar.height() - mSlider.height())  * (mPosition / mLenght); //relative width 

		mSlider.x(mSlideBar.x());
		mSlider.y(mSlideBar.y() + thumbPosition);
		r.drawImageRect(mSlider.x(), mSlider.y(), mSlider.width(), mSlider.height(), mSkinSlider);
	}
	else
	{
		r.drawImageRect(mSlideBar.x(), mSlideBar.y(), mSlideBar.width(), mSlideBar.height(), mSkinMiddle);				// slide area
		r.drawImageRect(mButton1.x(), mButton1.y(), mButton1.height(), mButton1.height(), mSkinButton1);	// left button
		r.drawImageRect(mButton2.x(), mButton2.y(), mButton2.height(), mButton2.height(), mSkinButton2);			// right button


		// Slider
		mSlider.height(mSlideBar.height());	// height = slide bar height
		mSlider.width(static_cast<int>(mSlideBar.width() / (mLenght+1))); //relative width 
		if (mSlider.width() < mSlider.height())	// not too relative. Minimum = Heigt itself
			mSlider.width(mSlider.height());
		thumbPosition = (mSlideBar.width() - mSlider.width())  * (mPosition / mLenght); //relative width 

		mSlider.x(mSlideBar.x() + thumbPosition);
		mSlider.y(mSlideBar.y());
		r.drawImageRect(mSlider.x(), mSlider.y(), mSlider.width(), mSlider.height(), mSkinSlider);
	}

	
	if (mDisplayPosition && mMouseHoverSlide){
		
		textHover = string_format("%i / %i",static_cast<int>(position()),static_cast<int>(mLenght));
		_w = font().width(textHover)+4;
		_h = font().height() + 4;
			
		if (mSliderType == SLIDER_VERTICAL){
			
			_x = mSlideBar.x() + mSlideBar.width() + 2;
			_y = mMouseY - _h;
		}else{
			_x = mMouseX + 2;
			_y = mSlideBar.y() - 2 - _h;
		}
		r.drawBox(_x, _y, _w, _h, 255, 255, 255, 180);
		r.drawBoxFilled(_x+1, _y+1, _w-2, _h-2, 0, 0, 0, 180);
		r.drawText(font(), textHover, _x+2, _y+2, 220, 220, 220);
		
	}
}

/**
 * Set the current value
 */
void Slider::position(double value)
{
	if (mBackward)
		value = mLenght - value;

	mPosition = value;
	
	if(mPosition < 0.0)
		mPosition = 0.0;
	else if(mPosition > mLenght)
		mPosition = mLenght;
	
	mCallback(position());
}

/**
* Gets the current value of position
*/
double Slider::position()
{
	double value = mPosition;
	if (mBackward)
		value = mLenght-value;
	return value;
}


/**
 * Adds the change amount to the current position.
 *
 * \param	change	Amount to change in percent to change the
 *					slider's position. Must be between 0.0
 *					1.0.
 */
void Slider::changePosition(double change)
{
	positionInternal(mPosition + change);
}

/**
 * Returns the max value position can get
 */
double Slider::length()
{
	return mLenght;
}

/**
* Set the max value position can get
*/
void Slider::length(double _lenght)
{
	mLenght = _lenght;
}