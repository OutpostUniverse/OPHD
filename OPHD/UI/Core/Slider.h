#pragma once

#include "Control.h"

#include <NAS2D/Timer.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Renderer/RectangleSkin.h>


 /**
 * Base class for all sliding controls.
 * 
 * Handle horizontal and vertical slide bar.
 * The handle could go backward or forward.
 * the handle width/height depend on the max value.
 * the minimum width/height is the height/width of the slide bar
 */
class Slider : public Control
{
public:
	/**
	 * List the types of slider that could be used
	 */
	enum class SliderType
	{
		Vertical, /**< Vertical slider. */
		Horizontal /**< Horizontal slider. */
	};

	struct Skins {
		NAS2D::RectangleSkin skinButton1;
		NAS2D::RectangleSkin skinMiddle;
		NAS2D::RectangleSkin skinButton2;
		NAS2D::RectangleSkin skinSlider;
	};

	using ValueChangeSignal = NAS2D::Signal<float>; /**< type for Signal on value changed. */

	Slider(SliderType sliderType = SliderType::Vertical);
	Slider(Skins skins, SliderType sliderType = SliderType::Vertical);
	~Slider() override;

	void thumbPosition(float value); /**< Set the current position. */
	float thumbPosition() const; /**< Get the current position. */
	void changeThumbPosition(float change); /**< Adds the change amount to the current position. */

	void thumbPositionNormalized(float value); /**< Set the current position. */
	float thumbPositionNormalized() const; /**< Get the current position. */

	bool displayPosition() const { return mDisplayPosition;} /**< Get the position display flag. */
	void displayPosition(bool value) { mDisplayPosition = value; } /**< Set the position display flag. */

	float length() const; /**< Get the max value for the slide area. */
	void length(float length); /**< Set the max value for the slide area. */

	bool backward() const { return mBackward; } /**< Get the backward flag. */
	void backward(bool isBackward) { mBackward = isBackward; } /**< Set the backward flag. */

	void update() override; /**< Called to display the slider. */

	ValueChangeSignal::Source& change() { return mSignal; } /**< Give the callback to enable another control or a window to dis/connect to this event call. */

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y); /**< Event raised on mouse button down. */
	virtual void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y); /**< Event raised on mouse button up. */
	virtual void onMouseMove(int x, int y, int dX, int dY); /**< Event raised on mouse move. */

private:
	float positionInternal();
	void positionInternal(float newPosition);

	void draw() const override; /**< Draw the widget on screen. */
	void logic(); /**< Compute some values before drawing the control. */

	void buttonCheck(bool& buttonFlag, NAS2D::Rectangle<int>& rect, float value);

	const NAS2D::Font& mFont;

	NAS2D::Timer mTimer;

	ValueChangeSignal mSignal; /**< Signal executed when the value is changed. */

	SliderType mSliderType{SliderType::Vertical}; /**< Type of the Slider. */

	// mouse event related vars
	NAS2D::Point<int> mMousePosition; /**< Mouse coordinates. */

	bool mMouseHoverSlide = false; /**< Mouse is within the bounds of the Button. */
	bool mThumbPressed = false; /**< Flag to indicate if this control is pressed. */

	// Slider values
	float mPosition = 0.0f; /**< Current value that represent the position of the slider. */
	float mLength = 0.0f; /**< Maximum value for the position of the slider. */

	bool mBackward = false; /**< Does the value returned in backward mode . */

	// Slider button responses
	uint32_t mPressedAccumulator = 0; /**< Accumulation value for pressed responses. */
	bool mButton1Held = false; /**< Flag indicating that a button is being held down. */
	bool mButton2Held = false; /**< Flag indicating that a button is being held down. */


	// drawing vars
	Skins mSkins;
	bool mDisplayPosition = false; /**< Indicate if the slider display the value on mouse over. */
	NAS2D::Rectangle<int> mButton1; /**< Area on screen where the second button is displayed. (Down/Left) */
	NAS2D::Rectangle<int> mButton2; /**< Area on screen where the first button is displayed. (Up/Right)*/
	NAS2D::Rectangle<int> mSlideBar; /**< Area on screen where the slide area is displayed. */
	NAS2D::Rectangle<int> mSlider; /**< Area on screen where the slider is displayed. */
};
