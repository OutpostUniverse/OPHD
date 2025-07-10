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
	setSelectedInternal(index);
}


void ListBoxBase::clearSelected()
{
	setSelectedInternal(NoSelection);
}


void ListBoxBase::clear()
{
	updateScrollLayout();

	mHighlightIndex = NoSelection;
	setSelectedInternal(NoSelection);
}


void ListBoxBase::setSelectedInternal(std::size_t index)
{
	if (mSelectedIndex != index)
	{
		mSelectedIndex = index;
		if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
	}
}


void ListBoxBase::updateScrollLayout()
{
	// Account for border around control
	mScrollArea = mRect.inset(1);

	const auto neededDisplaySize = mItemSize.y * static_cast<int>(count());
	if (neededDisplaySize > mScrollArea.size.y)
	{
		mScrollBar.size({14, mScrollArea.size.y});
		mScrollBar.position({mScrollArea.position.x + mScrollArea.size.x - mScrollBar.size().x, mScrollArea.position.y});
		mScrollBar.max(neededDisplaySize - mScrollArea.size.y);
		mScrollBar.largeDelta((mScrollArea.size.y / mItemSize.y) * mItemSize.y); // Pixel size of integral number of displayed items
		mScrollBar.visible(true);
		mScrollArea.size.x -= mScrollBar.size().x; // Remove scroll bar from scroll area
	}
	else
	{
		mScrollBar.max(0);
		mScrollBar.visible(false);
	}

	mItemSize.x = mScrollArea.size.x;
}


void ListBoxBase::onVisibilityChange(bool visible)
{
	if (visible)
	{
		const auto neededDisplaySize = mItemSize.y * static_cast<int>(count());
		mScrollBar.visible(neededDisplaySize > mScrollArea.size.y);
	}
}


void ListBoxBase::onMove(NAS2D::Vector<int> displacement)
{
	mScrollArea.position += displacement;
	mScrollBar.position(mScrollBar.position() + displacement);
}


void ListBoxBase::onResize()
{
	updateScrollLayout();
}


void ListBoxBase::onSlideChange(int newPosition)
{
	mScrollOffsetInPixels = newPosition;
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


NAS2D::Color ListBoxBase::borderColor() const
{
	return hasFocus() ? NAS2D::Color{0, 185, 0} : NAS2D::Color{75, 75, 75};
}


NAS2D::Color ListBoxBase::itemBorderColor(std::size_t /*index*/) const
{
	return {0, 185, 0};
}


NAS2D::Color ListBoxBase::emptyAreaColor() const
{
	return NAS2D::Color::Black;
}


void ListBoxBase::update()
{
	if (!visible()) { return; }

	draw();
	mScrollBar.update();
}


void ListBoxBase::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	renderer.drawBox(mRect, borderColor());
	drawScrollArea(renderer);
}


void ListBoxBase::drawScrollArea(NAS2D::Renderer& renderer) const
{
	renderer.clipRect(mScrollArea);

	// Determine visible items and draw them
	const auto lineHeight = mItemSize.y;
	const auto firstVisibleIndex = static_cast<std::size_t>(mScrollOffsetInPixels / lineHeight);
	const auto firstInvisibleIndex = static_cast<std::size_t>((mScrollOffsetInPixels + mScrollArea.size.y + (lineHeight - 1)) / lineHeight);
	const auto endVisibleIndex = std::min(firstInvisibleIndex, count());
	auto itemDrawArea = NAS2D::Rectangle{mScrollArea.position + NAS2D::Vector{0, static_cast<int>(firstVisibleIndex) * mItemSize.y - mScrollOffsetInPixels}, mItemSize};
	for (std::size_t index = firstVisibleIndex; index < endVisibleIndex; ++index)
	{
		drawItemCell(renderer, itemDrawArea, index);
		itemDrawArea.position.y += lineHeight;
	}

	// Paint remaining section of scroll area not covered by items
	itemDrawArea.size.y = mScrollArea.endPoint().y - itemDrawArea.position.y;
	renderer.drawBoxFilled(itemDrawArea, emptyAreaColor());

	renderer.clipRectClear();
}


void ListBoxBase::drawItemCell(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const
{
	const auto borderColor = itemBorderColor(index);

	// Draw background color
	const auto backgroundColor = (index == mHighlightIndex) ? NAS2D::Color{0, 36, 0} : NAS2D::Color::Black;
	renderer.drawBoxFilled(drawArea, backgroundColor);
	// Selected highlight
	if (index == mSelectedIndex) { renderer.drawBoxFilled(drawArea, borderColor.alphaFade(75)); }

	drawItem(renderer, drawArea, index);
}
