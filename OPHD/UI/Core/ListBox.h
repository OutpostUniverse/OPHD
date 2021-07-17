#pragma once

#include "Control.h"
#include "Slider.h"
#include "../../Cache.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Renderer/Renderer.h>

#include <string>
#include <vector>
#include <utility>
#include <cstddef>
#include <cmath>


namespace NAS2D
{
	class Font;
}


struct ListBoxItemText
{
	std::string text; /**< Text of the ListBoxItem. */
	int tag = 0; /**< User defined int data attached to the item. */

	struct Context
	{
		const NAS2D::Font& font;

		NAS2D::Color borderColorNormal = NAS2D::Color{75, 75, 75};
		NAS2D::Color borderColorActive = NAS2D::Color{0, 185, 0};

		NAS2D::Color itemBorderColorMouseHover = NAS2D::Color::DarkGreen;

		NAS2D::Color backgroundColorNormal = NAS2D::Color{0, 85, 0, 220};
		NAS2D::Color backgroundColorSelected = NAS2D::Color{0, 100, 0, 231};

		NAS2D::Color textColorNormal = NAS2D::Color::White;
		NAS2D::Color textColorMouseHover = NAS2D::Color::White;

		unsigned int itemHeight() const;
	};

	void draw(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> itemDrawArea, const Context& context, bool isSelected, bool isHighlighted);
};


/**
 * Implements a ListBox control.
 */
template <typename ListBoxItem = ListBoxItemText>
class ListBox : public Control
{
public:
	using SelectionChangeSignal = NAS2D::Signal<>;

	ListBox() :
		mContext{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)}
	{
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect(this, &ListBox::onMouseDown);
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect(this, &ListBox::onMouseMove);
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseWheel().connect(this, &ListBox::onMouseWheel);

		mSlider.displayPosition(false);
		mSlider.length(0);
		mSlider.thumbPosition(0);
		mSlider.change().connect(this, &ListBox::onSlideChange);
		updateScrollLayout();
	}

	~ListBox() override {
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().disconnect(this, &ListBox::onMouseDown);
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect(this, &ListBox::onMouseMove);
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseWheel().disconnect(this, &ListBox::onMouseWheel);

		mSlider.change().disconnect(this, &ListBox::onSlideChange);
	}

	bool isEmpty() const {
		return mItems.empty();
	}

	std::size_t count() const {
		return mItems.size();
	}

	template <typename... Args>
	void add(Args&&... args) {
		mItems.emplace_back(ListBoxItem{std::forward<Args>(args)...});
		updateScrollLayout();
	}

	void clear() {
		mItems.clear();
		mSelectedIndex = constants::NoSelection;
		mHighlightIndex = constants::NoSelection;
		updateScrollLayout();
	}


	bool isItemSelected() const {
		return mSelectedIndex != constants::NoSelection;
	}

	const ListBoxItem& selected() const {
		if (mSelectedIndex == constants::NoSelection)
		{
			throw std::runtime_error("ListBox has no selected item");
		}

		return mItems[mSelectedIndex];
	}


	std::size_t selectedIndex() const {
		return mSelectedIndex;
	}

	void setSelected(std::size_t index) {
		mSelectedIndex = index;
		mSelectionChanged();
	}

	void clearSelected() {
		mSelectedIndex = constants::NoSelection;
	}

	template <typename UnaryPredicate>
	void selectIf(UnaryPredicate predicate) {
		for (std::size_t i = 0; i < mItems.size(); ++i) {
			if (predicate(mItems[i])) {
				mSelectedIndex = i;
				return;
			}
		}
	}

	std::size_t currentHighlight() const {
		return mHighlightIndex;
	}

	unsigned int lineHeight() const {
		return mContext.itemHeight();
	}

	void update() override {
		// Ignore if menu is empty or invisible
		if (!visible()) { return; }

		auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

		const auto borderColor = hasFocus() ? mContext.borderColorActive : mContext.borderColorNormal;
		renderer.drawBox(mRect, borderColor);

		renderer.clipRect(mRect);

		// display actuals values that are meant to be
		const auto lineHeight = mContext.itemHeight();
		const auto firstVisibleIndex = mScrollOffsetInPixels / lineHeight;
		const auto lastVisibleIndex = (mScrollOffsetInPixels + mScrollArea.height + (lineHeight - 1)) / lineHeight;
		const auto endVisibleIndex = std::min(lastVisibleIndex, mItems.size());
		auto itemDrawArea = mScrollArea;
		itemDrawArea.y += -static_cast<int>(mScrollOffsetInPixels % lineHeight);
		itemDrawArea.height = static_cast<int>(lineHeight);
		for (std::size_t i = firstVisibleIndex; i < endVisibleIndex; i++)
		{
			const auto isSelected = (i == mSelectedIndex);
			const auto isHighlighted = (i == mHighlightIndex);

			mItems[i].draw(renderer, itemDrawArea, mContext, isSelected, isHighlighted);

			itemDrawArea.y += lineHeight;
		}

		// Paint remaining section of scroll area not covered by items
		itemDrawArea.height = mScrollArea.endPoint().y - itemDrawArea.startPoint().y;
		renderer.drawBoxFilled(itemDrawArea, mContext.backgroundColorNormal);

		renderer.clipRectClear();

		mSlider.update();
	}

	SelectionChangeSignal::Source& selectionChanged() {
		return mSelectionChanged;
	}

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton /*button*/, int x, int y) {
		if (!visible() || mHighlightIndex == constants::NoSelection || mHighlightIndex >= mItems.size() || !mScrollArea.contains({x, y}))
		{
			return;
		}

		setSelected(mHighlightIndex);
	}

	virtual void onMouseMove(int x, int y, int /*relX*/, int /*relY*/) {
		if (!visible() || !mScrollArea.contains({x, y}))
		{
			mHighlightIndex = constants::NoSelection;
			return;
		}

		mHighlightIndex = (static_cast<std::size_t>(y) - mScrollArea.y + mScrollOffsetInPixels) / static_cast<std::size_t>(mContext.itemHeight());
		if (mHighlightIndex >= mItems.size())
		{
			mHighlightIndex = constants::NoSelection;
		}
	}

	void onMouseWheel(int /*x*/, int y) {
		if (isEmpty() || !visible()) { return; }

		mSlider.changeThumbPosition((y < 0 ? 16.0f : -16.0f));
	}

	virtual void onSlideChange(float newPosition) {
		updateScrollLayout();
		// Intentional truncation of fractional value
		const auto pos = std::floor(newPosition);
		if (pos != newPosition)
		{
			mSlider.thumbPosition(pos);
		}
	}


	void onVisibilityChange(bool /*visible*/) override {
		updateScrollLayout();
	}

private:
	void onMove(NAS2D::Vector<int> /*displacement*/) override {
		updateScrollLayout();
	}

	void onResize() override {
		updateScrollLayout();
	}

	void updateScrollLayout() {
		// Account for border around control
		mScrollArea = mRect.inset(1);

		const auto neededDisplaySize = mContext.itemHeight() * mItems.size();
		if (neededDisplaySize > static_cast<std::size_t>(mRect.height))
		{
			mSlider.position({rect().x + mRect.width - 14, mRect.y});
			mSlider.size({14, mRect.height});
			mSlider.length(static_cast<float>(static_cast<int>(neededDisplaySize) - mRect.height));
			mScrollOffsetInPixels = static_cast<std::size_t>(mSlider.thumbPosition());
			mScrollArea.width -= mSlider.size().x; // Remove scroll bar from scroll area
			mSlider.visible(true);
		}
		else
		{
			mScrollOffsetInPixels = 0;
			mSlider.length(0);
			mSlider.visible(false);
		}
	}


	typename ListBoxItem::Context mContext;

	std::size_t mHighlightIndex = constants::NoSelection;
	std::size_t mSelectedIndex = 0;
	std::size_t mScrollOffsetInPixels = 0;

	std::vector<ListBoxItem> mItems; /**< List of items preserved in the order in which they're added. */

	NAS2D::Rectangle<int> mScrollArea;

	SelectionChangeSignal mSelectionChanged; /**< Signal for selection changed callback. */
	Slider mSlider;
};
