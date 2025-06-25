#pragma once

#include "Control.h"

#include <NAS2D/Timer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/RectangleSkin.h>
#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Signal/Delegate.h>


namespace NAS2D
{
	enum class MouseButton;
}


class ScrollBar : public Control
{
public:
	enum class ScrollBarType
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
	using ValueChangeDelegate = NAS2D::Delegate<void(ValueType)>;


	ScrollBar(ScrollBarType scrollBarType = ScrollBarType::Vertical, ValueChangeDelegate valueChangeHandler = {});
	ScrollBar(Skins skins, ScrollBarType scrollBarType = ScrollBarType::Vertical, ValueChangeDelegate valueChangeHandler = {});
	~ScrollBar() override;

	ValueType value() const;
	void value(ValueType newValue);
	void changeValue(ValueType change);

	ValueType max() const;
	void max(ValueType newMax);

	void update() override;
	ValueChangeSignal::Source& change() { return mSignal; }

protected:
	void draw() const override;

	void onButtonClick(bool& buttonFlag, ValueType value);
	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	void onMove(NAS2D::Vector<int> displacement) override;
	void onResize() override;
	void onLayoutChange();

private:
	ScrollBarType mScrollBarType{ScrollBarType::Vertical};
	ValueType mValue{0};
	ValueType mMax{0};
	ValueChangeSignal mSignal;

	// ScrollBar button responses
	NAS2D::Timer mTimer;
	uint32_t mPressedAccumulator{0}; /**< Accumulation value for pressed responses. */
	bool mThumbPressed{false}; /**< Flag to indicate if this control is pressed. */
	bool mButtonDecreaseHeld{false};
	bool mButtonIncreaseHeld{false};

	// Drawing vars
	Skins mSkins;
	NAS2D::Rectangle<int> mTrackRect;
	NAS2D::Rectangle<int> mThumbRect;
	NAS2D::Rectangle<int> mButtonDecreaseRect; // Top/Left
	NAS2D::Rectangle<int> mButtonIncreaseRect; // Bottom/Right
};
