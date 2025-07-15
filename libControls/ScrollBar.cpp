#include "ScrollBar.h"

#include "LoadRectangleSkin.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <tuple>


namespace
{
	ScrollBar::Skins loadSkinVertical()
	{
		return {
			loadRectangleSkin("ui/skin/scrollbar_vertical_track"),
			loadRectangleSkin("ui/skin/scrollbar_vertical_thumb"),
			loadRectangleSkin("ui/skin/scrollbar_vertical_button_decrease"),
			loadRectangleSkin("ui/skin/scrollbar_vertical_button_increase"),
		};
	}


	ScrollBar::Skins loadSkinHorizontal()
	{
		return {
			loadRectangleSkin("ui/skin/scrollbar_horizontal_track"),
			loadRectangleSkin("ui/skin/scrollbar_horizontal_thumb"),
			loadRectangleSkin("ui/skin/scrollbar_horizontal_button_decrease"),
			loadRectangleSkin("ui/skin/scrollbar_horizontal_button_increase"),
		};
	}


	ScrollBar::Skins loadSkins(ScrollBar::ScrollBarType scrollBarType)
	{
		return (scrollBarType == ScrollBar::ScrollBarType::Vertical) ?
			loadSkinVertical() : loadSkinHorizontal();
	}
}


ScrollBar::ScrollBar(ScrollBarType scrollBarType, int smallDelta, ValueChangeDelegate valueChangeHandler) :
	ScrollBar{loadSkins(scrollBarType), scrollBarType, smallDelta, valueChangeHandler}
{
}


ScrollBar::ScrollBar(ScrollBar::Skins skins, ScrollBarType scrollBarType, int smallDelta, ValueChangeDelegate valueChangeHandler) :
	mScrollBarType{scrollBarType},
	mSmallDelta{smallDelta},
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
	const auto newValueClamped = std::clamp(newValue, 0, mMax);
	if (mValue != newValueClamped)
	{
		mValue = newValueClamped;
		onThumbMove();
		if(mValueChangeHandler) { mValueChangeHandler(mValue); }
	}
}


void ScrollBar::changeValue(int valueDelta)
{
	value(mValue + valueDelta);
}


int ScrollBar::max() const
{
	return mMax;
}


void ScrollBar::max(int newMax)
{
	if (mMax != newMax)
	{
		mMax = newMax;
		onThumbResize();
	}
	value(mValue); // Re-clamp to new max
}


int ScrollBar::largeDelta() const
{
	return mLargeDelta;
}


void ScrollBar::largeDelta(int newLargeDelta)
{
	mLargeDelta = newLargeDelta;
	onThumbResize();
	onThumbMove();
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
			changeValue((mButtonDecreaseHeld ? -mSmallDelta : mSmallDelta));
		}
	}

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	draw(renderer);
}


void ScrollBar::draw(NAS2D::Renderer& renderer) const
{
	mSkins.skinTrack.draw(renderer, mTrackRect);
	mSkins.skinThumb.draw(renderer, mThumbRect);
	mSkins.skinButtonDecrease.draw(renderer, mButtonDecreaseRect);
	mSkins.skinButtonIncrease.draw(renderer, mButtonIncreaseRect);
}


void ScrollBar::onButtonClick(bool& buttonFlag, int valueDelta)
{
	changeValue(valueDelta);
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
			onButtonClick(mButtonDecreaseHeld, -mSmallDelta);
		}
		else if (mButtonIncreaseRect.contains(position))
		{
			onButtonClick(mButtonIncreaseHeld, mSmallDelta);
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
		const auto& [clickPosition, thumbPosition] =
			(mScrollBarType == ScrollBarType::Vertical) ?
				std::tuple{position.y, mThumbRect.position.y} :
				std::tuple{position.x, mThumbRect.position.x};
		const auto valueDelta = (clickPosition < thumbPosition) ?
			-mLargeDelta : mLargeDelta;
		changeValue(valueDelta);
	}
}


void ScrollBar::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (!enabled() || !visible()) { return; }

	if (mThumbPressed && mTrackRect.contains(position))
	{
		const auto [mouseTrackPosition, trackEmptySize] = (mScrollBarType == ScrollBarType::Vertical) ?
			std::tuple{position.y - mTrackRect.position.y - mThumbRect.size.y / 2, std::max(mTrackRect.size.y - mThumbRect.size.y, 1)} :
			std::tuple{position.x - mTrackRect.position.x - mThumbRect.size.x / 2, std::max(mTrackRect.size.x - mThumbRect.size.x, 1)};
		value(mMax * mouseTrackPosition / trackEmptySize);
	}
}


void ScrollBar::onMove(NAS2D::Vector<int> displacement)
{
	mTrackRect.position += displacement;
	mThumbRect.position += displacement;
	mButtonDecreaseRect.position += displacement;
	mButtonIncreaseRect.position += displacement;
}


void ScrollBar::onResize()
{
	if (mScrollBarType == ScrollBarType::Vertical)
	{
		const auto squareEndCapSize = NAS2D::Vector{mRect.size.x, mRect.size.x};
		mButtonDecreaseRect = {mRect.position, squareEndCapSize};
		mButtonIncreaseRect = {mRect.crossYPoint() - NAS2D::Vector{0, squareEndCapSize.y}, squareEndCapSize};
		mTrackRect = {mButtonDecreaseRect.crossYPoint(), mRect.size - NAS2D::Vector{0, squareEndCapSize.y * 2}};
	}
	else
	{
		const auto squareEndCapSize = NAS2D::Vector{mRect.size.y, mRect.size.y};
		mButtonDecreaseRect = {mRect.position, squareEndCapSize};
		mButtonIncreaseRect = {mRect.crossXPoint() - NAS2D::Vector{squareEndCapSize.x, 0}, squareEndCapSize};
		mTrackRect = {mButtonDecreaseRect.crossXPoint(), mRect.size - NAS2D::Vector{squareEndCapSize.x * 2, 0}};
	}
	onThumbResize();
	onThumbMove();
}


void ScrollBar::onThumbResize()
{
	if (mScrollBarType == ScrollBarType::Vertical)
	{
		const auto naturalThumbLength = mTrackRect.size.y * mLargeDelta / std::max(mMax + mLargeDelta, 1);
		const auto thumbLength = std::clamp(naturalThumbLength, mSkins.skinThumb.minSize().y, mTrackRect.size.y);
		mThumbRect.size = {mTrackRect.size.x, thumbLength};
	}
	else
	{
		const auto naturalThumbLength = mTrackRect.size.x * mLargeDelta / std::max(mMax + mLargeDelta, 1);
		const auto thumbLength = std::clamp(naturalThumbLength, mSkins.skinThumb.minSize().x, mTrackRect.size.x);
		mThumbRect.size = {thumbLength, mTrackRect.size.y};
	}
}


void ScrollBar::onThumbMove()
{
	if (mScrollBarType == ScrollBarType::Vertical)
	{
		const auto emptyTrackLength = mTrackRect.size.y - mThumbRect.size.y;
		const auto thumbOffset = emptyTrackLength * mValue / std::max(mMax, 1);
		mThumbRect.position = mTrackRect.position + NAS2D::Vector{0, thumbOffset};
	}
	else
	{
		const auto emptyTrackLength = mTrackRect.size.x - mThumbRect.size.x;
		const auto thumbOffset = emptyTrackLength * mValue / std::max(mMax, 1);
		mThumbRect.position = mTrackRect.position + NAS2D::Vector{thumbOffset, 0};
	}
}
