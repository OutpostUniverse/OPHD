#include "ScrollBar.h"

#include "LoadRectangleSkin.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>


namespace
{
	ScrollBar::Skins loadSkins(ScrollBar::ScrollBarType scrollBarType)
	{
		if (scrollBarType == ScrollBar::ScrollBarType::Vertical)
		{
			return {
				loadRectangleSkin("ui/skin/scrollbar_vertical_track"),
				loadRectangleSkin("ui/skin/scrollbar_vertical_thumb"),
				loadRectangleSkin("ui/skin/scrollbar_vertical_button_decrease"),
				loadRectangleSkin("ui/skin/scrollbar_vertical_button_increase"),
			};
		}
		else
		{
			return {
				loadRectangleSkin("ui/skin/scrollbar_horizontal_track"),
				loadRectangleSkin("ui/skin/scrollbar_horizontal_thumb"),
				loadRectangleSkin("ui/skin/scrollbar_horizontal_button_decrease"),
				loadRectangleSkin("ui/skin/scrollbar_horizontal_button_increase"),
			};
		}
	}
}


ScrollBar::ScrollBar(ScrollBarType scrollBarType, int smallChange, ValueChangeDelegate valueChangeHandler) :
	ScrollBar{loadSkins(scrollBarType), scrollBarType, smallChange, valueChangeHandler}
{
}


ScrollBar::ScrollBar(ScrollBar::Skins skins, ScrollBarType scrollBarType, int smallChange, ValueChangeDelegate valueChangeHandler) :
	mScrollBarType{scrollBarType},
	mSmallChange{smallChange},
	mValueChangeHandler{valueChangeHandler},
	mSkins{skins}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &ScrollBar::onMouseDown});
	eventHandler.mouseButtonUp().connect({this, &ScrollBar::onMouseUp});
	eventHandler.mouseMotion().connect({this, &ScrollBar::onMouseMove});
}


ScrollBar::~ScrollBar()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &ScrollBar::onMouseDown});
	eventHandler.mouseButtonUp().disconnect({this, &ScrollBar::onMouseUp});
	eventHandler.mouseMotion().disconnect({this, &ScrollBar::onMouseMove});
}


int ScrollBar::value() const
{
	return mValue;
}


void ScrollBar::value(int newValue)
{
	const auto oldValue = mValue;
	mValue = std::clamp(newValue, 0, mMax);
	if (mValue != oldValue)
	{
		if(mValueChangeHandler) { mValueChangeHandler(mValue); }
	}
}


void ScrollBar::changeValue(int change)
{
	value(mValue + change);
}


int ScrollBar::max() const
{
	return mMax;
}


void ScrollBar::max(int newMax)
{
	mMax = newMax;
	value(mValue); // Re-clamp to new max
}


void ScrollBar::update()
{
	if (!visible()) { return; }

	if (mButtonDecreaseHeld || mButtonIncreaseHeld)
	{
		if (mTimer.elapsedTicks() >= mPressedAccumulator)
		{
			mPressedAccumulator = 30;
			mTimer.reset();
			changeValue((mButtonDecreaseHeld ? -mSmallChange : mSmallChange));
		}
	}

	draw();
}


void ScrollBar::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	mSkins.skinTrack.draw(renderer, mTrackRect);
	mSkins.skinThumb.draw(renderer, mThumbRect);
	mSkins.skinButtonDecrease.draw(renderer, mButtonDecreaseRect);
	mSkins.skinButtonIncrease.draw(renderer, mButtonIncreaseRect);
}


void ScrollBar::onButtonClick(bool& buttonFlag, int value)
{
	changeValue(value);
	buttonFlag = true;

	mTimer.reset();
	mPressedAccumulator = 300;
}


void ScrollBar::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	if (button == NAS2D::MouseButton::Left)
	{
		if (mThumbRect.contains(position))
		{
			mThumbPressed = true;
		}
		else if (mButtonDecreaseRect.contains(position))
		{
			onButtonClick(mButtonDecreaseHeld, -mSmallChange);
		}
		else if (mButtonIncreaseRect.contains(position))
		{
			onButtonClick(mButtonIncreaseHeld, mSmallChange);
		}
	}
}


void ScrollBar::onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (button != NAS2D::MouseButton::Left) { return; }

	mButtonDecreaseHeld = false;
	mButtonIncreaseHeld = false;
	mThumbPressed = false;

	if (!enabled() || !visible()) { return; }

	if (mTrackRect.contains(position) && !mThumbRect.contains(position))
	{
		const auto& [clickPosition, thumbPosition, viewSize] =
			(mScrollBarType == ScrollBarType::Vertical) ?
				std::tuple{position.y, mThumbRect.position.y, mRect.size.y} :
				std::tuple{position.x, mThumbRect.position.x, mRect.size.x};
		const auto changeAmount = (clickPosition < thumbPosition) ?
			-viewSize : viewSize;
		changeValue(changeAmount);
	}
}


void ScrollBar::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (!enabled() || !visible()) { return; }

	if (mThumbPressed && mTrackRect.contains(position))
	{
		value(
			(mScrollBarType == ScrollBarType::Vertical) ?
				mMax * (position.y - mTrackRect.position.y - mThumbRect.size.y / 2) / std::max(mTrackRect.size.y - mThumbRect.size.y, 1) :
				mMax * (position.x - mTrackRect.position.x - mThumbRect.size.x / 2) / std::max(mTrackRect.size.x - mThumbRect.size.x, 1)
		);
	}
}


void ScrollBar::onMove(NAS2D::Vector<int> /*displacement*/)
{
	onLayoutChange();
}


void ScrollBar::onResize()
{
	onLayoutChange();
}


void ScrollBar::onLayoutChange()
{
	if (mScrollBarType == ScrollBarType::Vertical)
	{
		mButtonDecreaseRect = {mRect.position, {mRect.size.x, mRect.size.x}};
		mButtonIncreaseRect = {{mRect.position.x, mRect.position.y + mRect.size.y - mRect.size.x}, {mRect.size.x, mRect.size.x}};
		mTrackRect = {{mRect.position.x, mRect.position.y + mRect.size.x}, {mRect.size.x, mRect.size.y - 2 * mRect.size.x}};
		const auto newSize = std::min(mTrackRect.size.y * mRect.size.y / std::max(mMax + mRect.size.y, 1), mTrackRect.size.y);
		const auto drawOffset = (mTrackRect.size.y - newSize) * mValue / std::max(mMax, 1);
		mThumbRect = {{mTrackRect.position.x, mTrackRect.position.y + drawOffset}, {mTrackRect.size.x, newSize}};
	}
	else
	{
		mButtonDecreaseRect = {mRect.position, {mRect.size.y, mRect.size.y}};
		mButtonIncreaseRect = {{mRect.position.x + mRect.size.x - mRect.size.y, mRect.position.y}, {mRect.size.y, mRect.size.y}};
		mTrackRect = {{mRect.position.x + mRect.size.y, mRect.position.y}, {mRect.size.x - 2 * mRect.size.y, mRect.size.y}};
		const auto newSize = std::min(mTrackRect.size.x * mRect.size.x / std::max(mMax + mRect.size.x, 1), mTrackRect.size.x);
		const auto drawOffset = (mTrackRect.size.x - newSize) * mValue / std::max(mMax, 1);
		mThumbRect = {{mTrackRect.position.x + drawOffset, mTrackRect.position.y}, {newSize, mTrackRect.size.y}};
	}
}
