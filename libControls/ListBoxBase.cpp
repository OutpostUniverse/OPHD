#include "ListBoxBase.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Point.h>

#include <algorithm>
#include <limits>
#include <stdexcept>


const std::size_t ListBoxBase::NoSelection{std::numeric_limits<std::size_t>::max()};


ListBoxBase::ListBoxBase(NAS2D::Vector<int> itemSize, SelectionChangedDelegate selectionChangedHandler) :
	mScrollBar{ScrollBar::ScrollBarType::Vertical, itemSize.y, {this, &ListBoxBase::onSlideChange}},
	mItemSize{itemSize},
	mSelectionChangedHandler{selectionChangedHandler}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &ListBoxBase::onMouseDown});
	eventHandler.mouseMotion().connect({this, &ListBoxBase::onMouseMove});
	eventHandler.mouseWheel().connect({this, &ListBoxBase::onMouseWheel});
}


ListBoxBase::~ListBoxBase()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &ListBoxBase::onMouseDown});
	eventHandler.mouseMotion().disconnect({this, &ListBoxBase::onMouseMove});
	eventHandler.mouseWheel().disconnect({this, &ListBoxBase::onMouseWheel});
}


bool ListBoxBase::isEmpty() const
{
	return count() == 0;
}


bool ListBoxBase::isItemSelected() const
{
	return mSelectedIndex != NoSelection;
}


std::size_t ListBoxBase::highlightIndex() const
{
	return mHighlightIndex;
}


std::size_t ListBoxBase::selectedIndex() const
{
	return mSelectedIndex;
}


void ListBoxBase::selectedIndex(std::size_t index)
{
	if (index >= count())
	{
		throw std::runtime_error("Invalid list box selected index: " + std::to_string(index));
	}
	mSelectedIndex = index;
	if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
}


void ListBoxBase::clearSelected()
{
	mSelectedIndex = NoSelection;
	if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
}


void ListBoxBase::clear()
{
	mSelectedIndex = NoSelection;
	mHighlightIndex = NoSelection;
	updateScrollLayout();
}


void ListBoxBase::updateScrollLayout()
{
	// Account for border around control
	mScrollArea = mRect.inset(1);

	const auto neededDisplaySize = mItemSize.y * static_cast<int>(count());
	if (neededDisplaySize > mRect.size.y)
	{
		mScrollBar.size({14, mScrollArea.size.y});
		mScrollBar.position({mScrollArea.position.x + mScrollArea.size.x - mScrollBar.size().x, mScrollArea.position.y});
		mScrollBar.max(neededDisplaySize - mRect.size.y);
		mScrollOffsetInPixels = mScrollBar.value();
		mScrollArea.size.x -= mScrollBar.size().x; // Remove scroll bar from scroll area
		mScrollBar.visible(true);
	}
	else
	{
		mScrollOffsetInPixels = 0;
		mScrollBar.max(0);
		mScrollBar.visible(false);
	}

	mItemSize.x = mScrollArea.size.x;
}


void ListBoxBase::onVisibilityChange(bool visible)
{
	Control::onVisibilityChange(visible);

	updateScrollLayout();
}


void ListBoxBase::onResize()
{
	updateScrollLayout();
}


void ListBoxBase::onSlideChange(int /*newPosition*/)
{
	updateScrollLayout();
}


void ListBoxBase::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!visible() || !enabled() || !mRect.contains(position)) { return; }
	if (isEmpty() || button == NAS2D::MouseButton::Middle) { return; }

	if (button == NAS2D::MouseButton::Right)
	{
		clearSelected();
		return;
	}

	if (mHighlightIndex == NoSelection || !mScrollArea.contains(position)) { return; }
	selectedIndex(mHighlightIndex);
}


void ListBoxBase::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (!visible() || isEmpty()) { return; }

	mHasFocus = area().contains(position);

	if (!mScrollArea.contains(position))
	{
		mHighlightIndex = NoSelection;
		return;
	}

	const auto scrollRelativeY = mScrollOffsetInPixels + position.y - mScrollArea.position.y;
	const auto index = static_cast<std::size_t>(scrollRelativeY / mItemSize.y);
	mHighlightIndex = (index < count()) ? index : NoSelection;
}


void ListBoxBase::onMouseWheel(NAS2D::Vector<int> scrollAmount)
{
	if (!visible() || !hasFocus() || isEmpty()) { return; }

	mScrollBar.changeValue((scrollAmount.y < 0 ? mItemSize.y : -mItemSize.y));
}


NAS2D::Point<int> ListBoxBase::itemDrawPosition(std::size_t index) const
{
	return mScrollArea.position + NAS2D::Vector{0, static_cast<int>(index) * mItemSize.y - mScrollOffsetInPixels};
}


NAS2D::Rectangle<int> ListBoxBase::itemDrawArea(std::size_t index) const
{
	return {itemDrawPosition(index), mItemSize};
}


NAS2D::Color ListBoxBase::itemBorderColor(std::size_t /*index*/) const
{
	return {0, 185, 0};
}


void ListBoxBase::update()
{
	if (!visible()) { return; }
	mScrollBar.update();
	draw();
}


void ListBoxBase::drawItems() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// Determine visible items and draw them
	const auto lineHeight = mItemSize.y;
	const auto firstVisibleIndex = static_cast<std::size_t>(mScrollOffsetInPixels / lineHeight);
	const auto firstInvisibleIndex = static_cast<std::size_t>((mScrollOffsetInPixels + mRect.inset(1).size.y + (lineHeight - 1)) / lineHeight);
	const auto endVisibleIndex = std::min(firstInvisibleIndex, count());
	for (std::size_t index = firstVisibleIndex; index < endVisibleIndex; ++index)
	{
		const auto drawArea = itemDrawArea(index);
		const auto& borderColor = itemBorderColor(index);
		if (index == selectedIndex())
		{
			// Draw background highlight (drawn first to avoid tinting everything else)
			renderer.drawBoxFilled(drawArea, borderColor.alphaFade(75));
		}
		// Draw border
		renderer.drawBox(drawArea.inset(1), borderColor);

		drawItem(renderer, drawArea, index);
	}
}


void ListBoxBase::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// CONTROL EXTENTS
	renderer.drawBoxFilled(mScrollArea, NAS2D::Color::Black);
	renderer.drawBox(mRect, (hasFocus() ? NAS2D::Color{0, 185, 0} : NAS2D::Color{75, 75, 75}));

	renderer.clipRect(mRect.inset(1));

	// MOUSE HIGHLIGHT
	const auto highlightPosition = mScrollArea.position + NAS2D::Vector{0, static_cast<int>(mHighlightIndex) * mItemSize.y - mScrollOffsetInPixels};
	renderer.drawBoxFilled(NAS2D::Rectangle<int>{highlightPosition, mItemSize}, NAS2D::Color{0, 185, 0, 50});

	drawItems();

	renderer.clipRectClear();
}
