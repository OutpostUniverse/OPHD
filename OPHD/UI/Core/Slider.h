#pragma once

#include "Control.h"

#include <NAS2D/Timer.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Renderer/RectangleSkin.h>


class Slider : public Control
{
public:
	enum class SliderType
	{
		Vertical,
		Horizontal
	};

	struct Skins {
		NAS2D::RectangleSkin skinTrack;
		NAS2D::RectangleSkin skinThumb;
		NAS2D::RectangleSkin skinButtonDecrease;
		NAS2D::RectangleSkin skinButtonIncrease;
	};

	using ValueType = int;
	using ValueChangeSignal = NAS2D::Signal<ValueType>;

	Slider(SliderType sliderType = SliderType::Vertical);
	Slider(Skins skins, SliderType sliderType = SliderType::Vertical);
	~Slider() override;

	ValueType value() const;
	void value(ValueType newValue);
	void changeValue(ValueType change);

	ValueType max() const;
	void max(ValueType newMax);

	void update() override;
	void draw() const override;

	ValueChangeSignal::Source& change() { return mSignal; }

protected:
	void onButtonClick(bool& buttonFlag, ValueType value);
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int dX, int dY);

	void onMove(NAS2D::Vector<int> displacement) override;
	void onResize() override;
	void onLayoutChange();

private:
	SliderType mSliderType{SliderType::Vertical};
	ValueType mValue{0};
	ValueType mMax{0};
	ValueChangeSignal mSignal;

	// Slider button responses
	NAS2D::Timer mTimer;
	uint32_t mPressedAccumulator{0}; /**< Accumulation value for pressed responses. */
	bool mThumbPressed{false}; /**< Flag to indicate if this control is pressed. */
	bool mButtonDecreaseHeld{false};
	bool mButtonIncreaseHeld{false};

	// Drawing vars
	Skins mSkins;
	NAS2D::Rectangle<int> mTrack;
	NAS2D::Rectangle<int> mThumb;
	NAS2D::Rectangle<int> mButtonDecrease; // Top/Left
	NAS2D::Rectangle<int> mButtonIncrease; // Bottom/Right
};
