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
 * the handle width/height depend on the max value.
 * the minimum width/height is the height/width of the slide bar
 */
class Slider : public Control
{
public:
	enum class SliderType
	{
		Vertical,
		Horizontal
	};

	struct Skins {
		NAS2D::RectangleSkin skinButton1;
		NAS2D::RectangleSkin skinMiddle;
		NAS2D::RectangleSkin skinButton2;
		NAS2D::RectangleSkin skinSlider;
	};

	using ValueType = int;
	using ValueChangeSignal = NAS2D::Signal<ValueType>;

	Slider(SliderType sliderType = SliderType::Vertical);
	Slider(Skins skins, SliderType sliderType = SliderType::Vertical);
	~Slider() override;

	void value(ValueType newValue); /**< Set the current position. */
	ValueType value() const; /**< Get the current position. */
	void changeValue(ValueType change); /**< Adds the change amount to the current position. */

	ValueType length() const; /**< Get the max value for the slide area. */
	void length(ValueType length); /**< Set the max value for the slide area. */

	void update() override;

	ValueChangeSignal::Source& change() { return mSignal; }

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int dX, int dY);

	void draw() const override;
	void logic(); /**< Compute some values before drawing the control. */

	void buttonCheck(bool& buttonFlag, NAS2D::Rectangle<int>& rect, ValueType value);

private:
	const NAS2D::Font& mFont;

	NAS2D::Timer mTimer;

	ValueChangeSignal mSignal;

	SliderType mSliderType{SliderType::Vertical};

	// mouse event related vars
	NAS2D::Point<int> mMousePosition;

	bool mThumbPressed = false; /**< Flag to indicate if this control is pressed. */

	// Slider values
	ValueType mPosition = 0;
	ValueType mLength = 0;

	// Slider button responses
	uint32_t mPressedAccumulator = 0; /**< Accumulation value for pressed responses. */
	bool mButton1Held = false;
	bool mButton2Held = false;


	// drawing vars
	Skins mSkins;
	NAS2D::Rectangle<int> mButton1; /**< Area on screen where the second button is displayed. (Down/Left) */
	NAS2D::Rectangle<int> mButton2; /**< Area on screen where the first button is displayed. (Up/Right)*/
	NAS2D::Rectangle<int> mSlideBar; /**< Area on screen where the slide area is displayed. */
	NAS2D::Rectangle<int> mSlider; /**< Area on screen where the slider is displayed. */
};
