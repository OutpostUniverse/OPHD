#include "ScrollBar.h"

#include "../../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>


using namespace NAS2D;


namespace
{
	ScrollBar::Skins loadSkins(ScrollBar::ScrollBarType scrollBarType)
	{
		if (scrollBarType == ScrollBar::ScrollBarType::Vertical)
		{
			return {
				{ // Track
					imageCache.load("ui/skin/sv_sa_tl.png"),
					imageCache.load("ui/skin/sv_sa_tm.png"),
					imageCache.load("ui/skin/sv_sa_tr.png"),
					imageCache.load("ui/skin/sv_sa_ml.png"),
					imageCache.load("ui/skin/sv_sa_mm.png"),
					imageCache.load("ui/skin/sv_sa_mr.png"),
					imageCache.load("ui/skin/sv_sa_bl.png"),
					imageCache.load("ui/skin/sv_sa_bm.png"),
					imageCache.load("ui/skin/sv_sa_br.png")
				},
				{ // Thumb
					imageCache.load("ui/skin/sv_sl_tl.png"),
					imageCache.load("ui/skin/sv_sl_tm.png"),
					imageCache.load("ui/skin/sv_sl_tr.png"),
					imageCache.load("ui/skin/sv_sl_ml.png"),
					imageCache.load("ui/skin/sv_sl_mm.png"),
					imageCache.load("ui/skin/sv_sl_mr.png"),
					imageCache.load("ui/skin/sv_sl_bl.png"),
					imageCache.load("ui/skin/sv_sl_bm.png"),
					imageCache.load("ui/skin/sv_sl_br.png")
				},
				{ // ButtonDecrease
					imageCache.load("ui/skin/sv_bu_tl.png"),
					imageCache.load("ui/skin/sv_bu_tm.png"),
					imageCache.load("ui/skin/sv_bu_tr.png"),
					imageCache.load("ui/skin/sv_bu_ml.png"),
					imageCache.load("ui/skin/sv_bu_mm.png"),
					imageCache.load("ui/skin/sv_bu_mr.png"),
					imageCache.load("ui/skin/sv_bu_bl.png"),
					imageCache.load("ui/skin/sv_bu_bm.png"),
					imageCache.load("ui/skin/sv_bu_br.png")
				},
				{ // ButtonIncrease
					imageCache.load("ui/skin/sv_bd_tl.png"),
					imageCache.load("ui/skin/sv_bd_tm.png"),
					imageCache.load("ui/skin/sv_bd_tr.png"),
					imageCache.load("ui/skin/sv_bd_ml.png"),
					imageCache.load("ui/skin/sv_bd_mm.png"),
					imageCache.load("ui/skin/sv_bd_mr.png"),
					imageCache.load("ui/skin/sv_bd_bl.png"),
					imageCache.load("ui/skin/sv_bd_bm.png"),
					imageCache.load("ui/skin/sv_bd_br.png")
				},
			};
		}
		else
		{
			return {
				{ // Track
					imageCache.load("ui/skin/sh_sa_tl.png"),
					imageCache.load("ui/skin/sh_sa_tm.png"),
					imageCache.load("ui/skin/sh_sa_tr.png"),
					imageCache.load("ui/skin/sh_sa_ml.png"),
					imageCache.load("ui/skin/sh_sa_mm.png"),
					imageCache.load("ui/skin/sh_sa_mr.png"),
					imageCache.load("ui/skin/sh_sa_bl.png"),
					imageCache.load("ui/skin/sh_sa_bm.png"),
					imageCache.load("ui/skin/sh_sa_br.png")
				},
				{ // Thumb
					imageCache.load("ui/skin/sh_sl_tl.png"),
					imageCache.load("ui/skin/sh_sl_tm.png"),
					imageCache.load("ui/skin/sh_sl_tr.png"),
					imageCache.load("ui/skin/sh_sl_ml.png"),
					imageCache.load("ui/skin/sh_sl_mm.png"),
					imageCache.load("ui/skin/sh_sl_mr.png"),
					imageCache.load("ui/skin/sh_sl_bl.png"),
					imageCache.load("ui/skin/sh_sl_bm.png"),
					imageCache.load("ui/skin/sh_sl_br.png")
				},
				{ // ButtonDecrease
					imageCache.load("ui/skin/sh_bl_tl.png"),
					imageCache.load("ui/skin/sh_bl_tm.png"),
					imageCache.load("ui/skin/sh_bl_tr.png"),
					imageCache.load("ui/skin/sh_bl_ml.png"),
					imageCache.load("ui/skin/sh_bl_mm.png"),
					imageCache.load("ui/skin/sh_bl_mr.png"),
					imageCache.load("ui/skin/sh_bl_bl.png"),
					imageCache.load("ui/skin/sh_bl_bm.png"),
					imageCache.load("ui/skin/sh_bl_br.png")
				},
				{ // ButtonIncrease
					imageCache.load("ui/skin/sh_br_tl.png"),
					imageCache.load("ui/skin/sh_br_tm.png"),
					imageCache.load("ui/skin/sh_br_tr.png"),
					imageCache.load("ui/skin/sh_br_ml.png"),
					imageCache.load("ui/skin/sh_br_mm.png"),
					imageCache.load("ui/skin/sh_br_mr.png"),
					imageCache.load("ui/skin/sh_br_bl.png"),
					imageCache.load("ui/skin/sh_br_bm.png"),
					imageCache.load("ui/skin/sh_br_br.png")
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
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().connect(this, &ScrollBar::onMouseDown);
	eventHandler.mouseButtonUp().connect(this, &ScrollBar::onMouseUp);
	eventHandler.mouseMotion().connect(this, &ScrollBar::onMouseMove);
}


ScrollBar::~ScrollBar()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect(this, &ScrollBar::onMouseDown);
	eventHandler.mouseButtonUp().disconnect(this, &ScrollBar::onMouseUp);
	eventHandler.mouseMotion().disconnect(this, &ScrollBar::onMouseMove);
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
		if (mTimer.accumulator() >= mPressedAccumulator)
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
	auto& renderer = Utility<Renderer>::get();

	mSkins.skinTrack.draw(renderer, mTrack);
	mSkins.skinThumb.draw(renderer, mThumb);
	mSkins.skinButtonDecrease.draw(renderer, mButtonDecrease);
	mSkins.skinButtonIncrease.draw(renderer, mButtonIncrease);
}


void ScrollBar::onButtonClick(bool& buttonFlag, ValueType value)
{
	changeValue(value);
	buttonFlag = true;

	mTimer.reset();
	mPressedAccumulator = 300;
}


void ScrollBar::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!enabled() || !visible()) { return; }

	if (button == EventHandler::MouseButton::Left)
	{
		const auto mousePosition = NAS2D::Point{x, y};
		if (mThumb.contains(mousePosition))
		{
			mThumbPressed = true;
		}
		else if (mButtonDecrease.contains(mousePosition))
		{
			onButtonClick(mButtonDecreaseHeld, -1);
		}
		else if (mButtonIncrease.contains(mousePosition))
		{
			onButtonClick(mButtonIncreaseHeld, 1);
		}
	}
}


void ScrollBar::onMouseUp(EventHandler::MouseButton button, int x, int y)
{
	if (button != EventHandler::MouseButton::Left) { return; }

	mButtonDecreaseHeld = false;
	mButtonIncreaseHeld = false;
	mThumbPressed = false;

	if (!enabled() || !visible()) { return; }

	const auto mousePosition = NAS2D::Point{x, y};
	if (mTrack.contains(mousePosition) && !mThumb.contains(mousePosition))
	{
		const auto [clickPosition, thumbPosition, viewSize] =
			(mScrollBarType == ScrollBarType::Vertical) ?
				std::tuple{y, mThumb.y, mRect.height} : std::tuple{x, mThumb.x, mRect.width};
		const auto changeAmount = (clickPosition < thumbPosition) ?
			-viewSize : viewSize;
		changeValue(changeAmount);
	}
}


void ScrollBar::onMouseMove(int x, int y, int /*dX*/, int /*dY*/)
{
	if (!enabled() || !visible()) { return; }

	if (mThumbPressed && mTrack.contains({x, y}))
	{
		value(
			(mScrollBarType == ScrollBarType::Vertical) ?
				mMax * (y - mTrack.y - mThumb.height / 2) / (mTrack.height - mThumb.height) :
				mMax * (x - mTrack.x - mThumb.width / 2) / (mTrack.width - mThumb.width)
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
		mButtonDecrease = {mRect.x, mRect.y, mRect.width, mRect.width};
		mButtonIncrease = {mRect.x, mRect.y + mRect.height - mRect.width, mRect.width, mRect.width};
		mTrack = {mRect.x, mRect.y + mRect.width, mRect.width, mRect.height - 2 * mRect.width};
		const auto newSize = std::min(mTrack.height * mRect.height / std::max(mMax + mRect.height, 1), mTrack.height);
		const auto drawOffset = (mTrack.height - newSize) * mValue / std::max(mMax, 1);
		mThumb = {mTrack.x, mTrack.y + drawOffset, mTrack.width, newSize};
	}
	else
	{
		mButtonDecrease = {mRect.x, mRect.y, mRect.height, mRect.height};
		mButtonIncrease = {mRect.x + mRect.width - mRect.height, mRect.y, mRect.height, mRect.height};
		mTrack = {mRect.x + mRect.height, mRect.y, mRect.width - 2 * mRect.height, mRect.height};
		const auto newSize = std::min(mTrack.width * mRect.width / std::max(mMax + mRect.width, 1), mTrack.width);
		const auto drawOffset = (mTrack.width - newSize) * mValue / std::max(mMax, 1);
		mThumb = {mTrack.x + drawOffset, mTrack.y, newSize, mTrack.height};
	}
}
