/*!
 * \file	Slider.h
 * \brief	Header for sliding controls.
 * \author Goof
 */
#pragma once
#include "Button.h"
#include "Control.h"
#include "../../Common.h"


 /**
 * \class	Slider
 * \brief	Base class for all sliding controls.
 * 
 * Handle horizontal and vertical slide bar.
 * The handle could go backward or forward.
 * the handle width/height depend on the max value.
 * the minimum width/height is the height/width of the slide bar
 */
class Slider : public Control
{
public:
	/*!
	 * \enum 	SliderType
	 * \brief	List the types of slider that could be used
	 */
	enum SliderType
	{
		SLIDER_VERTICAL,	/*!< Vertical slider. */
		SLIDER_HORIZONTAL	/*!< Horizontal slider. */
	};

	typedef NAS2D::Signals::Signal1<double> ValueChangedCallback; /*!< type for Callback on value changed. */

public:
	Slider();
	virtual ~Slider();

	void thumbPosition(double value);		/*!< Set the current position. */
	double thumbPosition();					/*!< Get the current position. */
	void changeThumbPosition(double change);	/*!< Adds the change amount to the current position. */

	bool displayPosition() { return mDisplayPosition;}			/*!< Get the position display flag. */
	void displayPosition(bool _d) { mDisplayPosition = _d; }	/*!< Set the position display flag. */

	double length(); 			/*!< Get the max value for the slide area. */
	void length(double _lengh);	/*!< Set the max value for the slide area. */
	
	bool backward() { return mBackward; }	 	/*!< Get the backward flag. */
	void backward(bool _b) { mBackward = _b; } 	/*!< Set the backward flag. */

	void update(); 							/*!< Called to display the slider. */
	virtual void size(float w, float h); 	/*!< Set the slider size. */

	ValueChangedCallback& change() { return mCallback; } 	/*!< Give the callback to enable another control or a window to dis/connect to this event call. */

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y); 	/*!< Event raised on mouse button down. */
	virtual void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y); 	/*!< Event raised on mouse button up. */
	virtual void onMouseMotion(int x, int y, int dX, int dY); 	/*!< Event raised on mouse move. */
	
private:
	double positionInternal();
	void positionInternal(double _pos);

	void setSkins();	/*!< Helper function that load the gui skin on the first update call. */
	void draw();		/*!< Draw the widget on screen. */
	void logic();		/*!< Compute some values before drawing the control. */

	void _buttonCheck(bool& buttonFlag, NAS2D::Rectangle_2df& rect, double value);

private:
	NAS2D::Timer			mTimer;

	ValueChangedCallback	mCallback;					/*!< Callback executed when the value is changed. */

	SliderType				mSliderType;				/*!< Type of the Slider. */
	
	// mouse event related vars
	NAS2D::Point_2d			mMousePosition;				/**< Mouse coordinates. */

	bool					mMouseHoverSlide = false;	/*!< Mouse is within the bounds of the Button. */
	bool					mThumbPressed = false;		/*!< Flag to indicate if this control is pressed. */

    // Slider values
	double					mPosition = 0.0;			/*!< Current value that represent the position of the slider. */
	double					mLenght = 0.0;				/*!< Maximum value for the position of the slider. */

	bool					mBackward = false;			/*!< Does the value returned in backward mode . */

	// Slider button responses
	uint32_t				mPressedAccumulator = 0;	/**< Accumulation value for pressed responses. */
	bool					mButton1Held = false;		/**< Flag indicating that a button is being held down. */
	bool					mButton2Held = false;		/**< Flag indicating that a button is being held down. */


	// drawing vars
	NAS2D::ImageList		mSkinButton1;				/*!< Skin for button 1 (Up or Left). */
	NAS2D::ImageList		mSkinButton2;				/*!< Skin for button 2 (Down or Right). */
	NAS2D::ImageList		mSkinMiddle;				/*!< Skin for the slide area. */
	
	NAS2D::ImageList		mSkinSlider;				/*!< Skin for the slider. */
	bool					mDisplayPosition = false;	/*!< Indicate if the slider display the value on mouse over. */
	
	NAS2D::Rectangle_2df	mButton1;					/*!< Area on screen where the second button is displayed. (Down/Left) */
	NAS2D::Rectangle_2df	mButton2;					/*!< Area on screen where the first button is displayed. (Up/Right)*/
	NAS2D::Rectangle_2df	mSlideBar;					/*!< Area on screen where the slide area is displayed. */
	NAS2D::Rectangle_2df	mSlider;					/*!< Area on screen where the slider is displayed. */
};
