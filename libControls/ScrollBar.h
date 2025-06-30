#pragma once

#include "Control.h"

#include <NAS2D/Timer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/RectangleSkin.h>
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

	using ValueChangeDelegate = NAS2D::Delegate<void(int)>;


	ScrollBar(ScrollBarType scrollBarType = ScrollBarType::Vertical, int smallChange = 1, ValueChangeDelegate valueChangeHandler = {});
	ScrollBar(Skins skins, ScrollBarType scrollBarType = ScrollBarType::Vertical, int smallChange = 1, ValueChangeDelegate valueChangeHandler = {});
	~ScrollBar() override;

	int value() const;
	void value(int newValue);
	void changeValue(int change);

	int max() const;
	void max(int newMax);

	void update() override;

protected:
	void draw() const override;

	void onButtonClick(bool& buttonFlag, int value);
	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	void onMove(NAS2D::Vector<int> displacement) override;
	void onResize() override;
	void onLayoutChange();

private:
	const ScrollBarType mScrollBarType;
	const int mSmallChange;
	int mMax{0};
	int mValue{0};
	ValueChangeDelegate mValueChangeHandler;

	// ScrollBar button responses
	NAS2D::Timer mTimer;
	uint32_t mPressedAccumulator{0}; /**< Accumulation value for pressed responses. */
	bool mThumbPressed{false}; /**< Flag to indicate if this control is pressed. */
	bool mButtonDecreaseHeld{false};
	bool mButtonIncreaseHeld{false};

	// Drawing vars
	const Skins mSkins;
	NAS2D::Rectangle<int> mTrackRect;
	NAS2D::Rectangle<int> mThumbRect;
	NAS2D::Rectangle<int> mButtonDecreaseRect; // Top/Left
	NAS2D::Rectangle<int> mButtonIncreaseRect; // Bottom/Right
};
