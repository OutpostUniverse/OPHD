#include "ScrollBar.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>


namespace
{
	ScrollBar::Skins loadSkins(ScrollBar::ScrollBarType scrollBarType)
	{
		if (scrollBarType == ScrollBar::ScrollBarType::Vertical)
		{
			return {
				{ // Track
					Control::getImage("ui/skin/sv_sa_tl.png"),
					Control::getImage("ui/skin/sv_sa_tm.png"),
					Control::getImage("ui/skin/sv_sa_tr.png"),
					Control::getImage("ui/skin/sv_sa_ml.png"),
					Control::getImage("ui/skin/sv_sa_mm.png"),
					Control::getImage("ui/skin/sv_sa_mr.png"),
					Control::getImage("ui/skin/sv_sa_bl.png"),
					Control::getImage("ui/skin/sv_sa_bm.png"),
					Control::getImage("ui/skin/sv_sa_br.png")
				},
				{ // Thumb
					Control::getImage("ui/skin/sv_sl_tl.png"),
					Control::getImage("ui/skin/sv_sl_tm.png"),
					Control::getImage("ui/skin/sv_sl_tr.png"),
					Control::getImage("ui/skin/sv_sl_ml.png"),
					Control::getImage("ui/skin/sv_sl_mm.png"),
					Control::getImage("ui/skin/sv_sl_mr.png"),
					Control::getImage("ui/skin/sv_sl_bl.png"),
					Control::getImage("ui/skin/sv_sl_bm.png"),
					Control::getImage("ui/skin/sv_sl_br.png")
				},
				{ // ButtonDecrease
					Control::getImage("ui/skin/sv_bu_tl.png"),
					Control::getImage("ui/skin/sv_bu_tm.png"),
					Control::getImage("ui/skin/sv_bu_tr.png"),
					Control::getImage("ui/skin/sv_bu_ml.png"),
					Control::getImage("ui/skin/sv_bu_mm.png"),
					Control::getImage("ui/skin/sv_bu_mr.png"),
					Control::getImage("ui/skin/sv_bu_bl.png"),
					Control::getImage("ui/skin/sv_bu_bm.png"),
					Control::getImage("ui/skin/sv_bu_br.png")
				},
				{ // ButtonIncrease
					Control::getImage("ui/skin/sv_bd_tl.png"),
					Control::getImage("ui/skin/sv_bd_tm.png"),
					Control::getImage("ui/skin/sv_bd_tr.png"),
					Control::getImage("ui/skin/sv_bd_ml.png"),
					Control::getImage("ui/skin/sv_bd_mm.png"),
					Control::getImage("ui/skin/sv_bd_mr.png"),
					Control::getImage("ui/skin/sv_bd_bl.png"),
					Control::getImage("ui/skin/sv_bd_bm.png"),
					Control::getImage("ui/skin/sv_bd_br.png")
				},
			};
		}
		else
		{
			return {
				{ // Track
					Control::getImage("ui/skin/sh_sa_tl.png"),
					Control::getImage("ui/skin/sh_sa_tm.png"),
					Control::getImage("ui/skin/sh_sa_tr.png"),
					Control::getImage("ui/skin/sh_sa_ml.png"),
					Control::getImage("ui/skin/sh_sa_mm.png"),
					Control::getImage("ui/skin/sh_sa_mr.png"),
					Control::getImage("ui/skin/sh_sa_bl.png"),
					Control::getImage("ui/skin/sh_sa_bm.png"),
					Control::getImage("ui/skin/sh_sa_br.png")
				},
				{ // Thumb
					Control::getImage("ui/skin/sh_sl_tl.png"),
					Control::getImage("ui/skin/sh_sl_tm.png"),
					Control::getImage("ui/skin/sh_sl_tr.png"),
					Control::getImage("ui/skin/sh_sl_ml.png"),
					Control::getImage("ui/skin/sh_sl_mm.png"),
					Control::getImage("ui/skin/sh_sl_mr.png"),
					Control::getImage("ui/skin/sh_sl_bl.png"),
					Control::getImage("ui/skin/sh_sl_bm.png"),
					Control::getImage("ui/skin/sh_sl_br.png")
				},
				{ // ButtonDecrease
					Control::getImage("ui/skin/sh_bl_tl.png"),
					Control::getImage("ui/skin/sh_bl_tm.png"),
					Control::getImage("ui/skin/sh_bl_tr.png"),
					Control::getImage("ui/skin/sh_bl_ml.png"),
					Control::getImage("ui/skin/sh_bl_mm.png"),
					Control::getImage("ui/skin/sh_bl_mr.png"),
					Control::getImage("ui/skin/sh_bl_bl.png"),
					Control::getImage("ui/skin/sh_bl_bm.png"),
					Control::getImage("ui/skin/sh_bl_br.png")
				},
				{ // ButtonIncrease
					Control::getImage("ui/skin/sh_br_tl.png"),
					Control::getImage("ui/skin/sh_br_tm.png"),
					Control::getImage("ui/skin/sh_br_tr.png"),
					Control::getImage("ui/skin/sh_br_ml.png"),
					Control::getImage("ui/skin/sh_br_mm.png"),
					Control::getImage("ui/skin/sh_br_mr.png"),
					Control::getImage("ui/skin/sh_br_bl.png"),
					Control::getImage("ui/skin/sh_br_bm.png"),
					Control::getImage("ui/skin/sh_br_br.png")
				},
			};
		}
	}
}


ScrollBar::ScrollBar(ScrollBarType scrollBarType) :
	ScrollBar{loadSkins(scrollBarType), scrollBarType}
{}


ScrollBar::ScrollBar(ScrollBar::Skins skins, ScrollBarType scrollBarType) :
	mScrollBarType{scrollBarType},
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


ScrollBar::ValueType ScrollBar::value() const
{
	return mValue;
}


void ScrollBar::value(ValueType newValue)
{
	const auto oldValue = mValue;
	mValue = std::clamp<ValueType>(newValue, 0, mMax);
	if (mValue != oldValue)
	{
		mSignal(mValue);
	}
}


void ScrollBar::changeValue(ValueType change)
{
	value(mValue + change);
}


ScrollBar::ValueType ScrollBar::max() const
{
	return mMax;
}


void ScrollBar::max(ValueType newMax)
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
			changeValue((mButtonDecreaseHeld ? -1 : 1));
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


void ScrollBar::onButtonClick(bool& buttonFlag, ValueType value)
{
	changeValue(value);
	buttonFlag = true;

	mTimer.reset();
	mPressedAccumulator = 300;
}


void ScrollBar::onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	if (button == NAS2D::EventHandler::MouseButton::Left)
	{
		if (mThumbRect.contains(position))
		{
			mThumbPressed = true;
		}
		else if (mButtonDecreaseRect.contains(position))
		{
			onButtonClick(mButtonDecreaseHeld, -1);
		}
		else if (mButtonIncreaseRect.contains(position))
		{
			onButtonClick(mButtonIncreaseHeld, 1);
		}
	}
}


void ScrollBar::onMouseUp(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (button != NAS2D::EventHandler::MouseButton::Left) { return; }

	mButtonDecreaseHeld = false;
	mButtonIncreaseHeld = false;
	mThumbPressed = false;

	if (!enabled() || !visible()) { return; }

	if (mTrackRect.contains(position) && !mThumbRect.contains(position))
	{
		const auto [clickPosition, thumbPosition, viewSize] =
			(mScrollBarType == ScrollBarType::Vertical) ?
				std::tuple{position.y, mThumbRect.position.y, mRect.size.y} : std::tuple{position.x, mThumbRect.position.x, mRect.size.x};
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
				mMax * (position.y - mTrackRect.position.y - mThumbRect.size.y / 2) / (mTrackRect.size.y - mThumbRect.size.y) :
				mMax * (position.x - mTrackRect.position.x - mThumbRect.size.x / 2) / (mTrackRect.size.x - mThumbRect.size.x)
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
