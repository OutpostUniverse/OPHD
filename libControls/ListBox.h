#pragma once

#include "Control.h"
#include "ScrollBar.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Renderer/Renderer.h>

#include <string>
#include <vector>
#include <utility>
#include <cstddef>
#include <limits>


namespace NAS2D
{
	class Font;
}


struct ListBoxItemText
{
	std::string text;
	int userData = 0;

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

		int itemHeight() const;
	};

	void draw(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> itemDrawRect, const Context& context, bool isSelected, bool isHighlighted) const;

	bool operator<(const ListBoxItemText& other) const
	{
		return text < other.text;
	}
};


template <typename ListBoxItem = ListBoxItemText>
class ListBox : public Control
{
public:
	using Context = typename ListBoxItem::Context;
	using SelectionChangedDelegate = NAS2D::Delegate<void()>;

	static inline constexpr auto NoSelection{std::numeric_limits<std::size_t>::max()};


	ListBox(SelectionChangedDelegate selectionChangedHandler = {}) :
		mContext{getDefaultFont()},
		mScrollBar{ScrollBar::ScrollBarType::Vertical, mContext.itemHeight(), {this, &ListBox::onSlideChange}},
		mItemSize{0, mContext.itemHeight()},
		mSelectionChangedHandler{selectionChangedHandler}
	{
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &ListBox::onMouseDown});
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect({this, &ListBox::onMouseMove});
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseWheel().connect({this, &ListBox::onMouseWheel});

		updateScrollLayout();
	}


	~ListBox() override
	{
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().disconnect({this, &ListBox::onMouseDown});
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect({this, &ListBox::onMouseMove});
		NAS2D::Utility<NAS2D::EventHandler>::get().mouseWheel().disconnect({this, &ListBox::onMouseWheel});
	}


	bool isEmpty() const
	{
		return mItems.empty();
	}


	std::size_t count() const
	{
		return mItems.size();
	}


	template <typename... Args>
	void add(Args&&... args)
	{
		mItems.emplace_back(ListBoxItem{std::forward<Args>(args)...});
		updateScrollLayout();
	}


	void clear()
	{
		mItems.clear();
		mSelectedIndex = NoSelection;
		mHighlightIndex = NoSelection;
		updateScrollLayout();
	}


	bool isItemSelected() const
	{
		return mSelectedIndex != NoSelection;
	}

	const ListBoxItem& selected() const
	{
		if (mSelectedIndex == NoSelection)
		{
			throw std::runtime_error("ListBox has no selected item");
		}

		return mItems[mSelectedIndex];
	}


	std::size_t selectedIndex() const
	{
		return mSelectedIndex;
	}


	void setSelected(std::size_t index)
	{
		mSelectedIndex = index;
		if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
	}


	void clearSelected()
	{
		mSelectedIndex = NoSelection;
		if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
	}

	template <typename UnaryPredicate>
	void selectIf(UnaryPredicate predicate)
	{
		for (std::size_t i = 0; i < count(); ++i)
		{
			if (predicate(mItems[i]))
			{
				mSelectedIndex = i;
				return;
			}
		}
	}


	std::size_t currentHighlight() const
	{
		return mHighlightIndex;
	}


	int itemHeight() const
	{
		return mItemSize.y;
	}


	void update() override
	{
		if (!visible()) { return; }

		draw();
		mScrollBar.update();
	}


protected:
	void draw() const override
	{
		auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

		const auto borderColor = hasFocus() ? mContext.borderColorActive : mContext.borderColorNormal;
		renderer.drawBox(mRect, borderColor);

		renderer.clipRect(mScrollArea);

		// Determine visible items and draw them
		const auto lineHeight = mItemSize.y;
		const auto firstVisibleIndex = static_cast<std::size_t>(mScrollOffsetInPixels / lineHeight);
		const auto firstInvisibleIndex = static_cast<std::size_t>((mScrollOffsetInPixels + mScrollArea.size.y + (lineHeight - 1)) / lineHeight);
		const auto endVisibleIndex = std::min(firstInvisibleIndex, count());
		auto itemDrawRect = mScrollArea;
		itemDrawRect.position.y += -(mScrollOffsetInPixels % lineHeight);
		itemDrawRect.size.y = lineHeight;
		for (std::size_t index = firstVisibleIndex; index < endVisibleIndex; ++index)
		{
			const auto isSelected = (index == mSelectedIndex);
			const auto isHighlighted = (index == mHighlightIndex);

			mItems[index].draw(renderer, itemDrawRect, mContext, isSelected, isHighlighted);

			itemDrawRect.position.y += lineHeight;
		}

		// Paint remaining section of scroll area not covered by items
		itemDrawRect.size.y = mScrollArea.endPoint().y - itemDrawRect.position.y;
		renderer.drawBoxFilled(itemDrawRect, mContext.backgroundColorNormal);

		renderer.clipRectClear();
	}


	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
	{
		if (!visible() || !enabled() || !mRect.contains(position)) { return; }
		if (isEmpty() || button == NAS2D::MouseButton::Middle) { return; }

		if (button == NAS2D::MouseButton::Right)
		{
			clearSelected();
			return;
		}

		if (mHighlightIndex == NoSelection || !mScrollArea.contains(position)) { return; }
		setSelected(mHighlightIndex);
	}


	virtual void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
	{
		if (!visible() || isEmpty()) { return; }

		if (!mScrollArea.contains(position))
		{
			mHighlightIndex = NoSelection;
			return;
		}

		const auto scrollRelativeY = mScrollOffsetInPixels + position.y - mScrollArea.position.y;
		const auto index = static_cast<std::size_t>(scrollRelativeY / mItemSize.y);
		mHighlightIndex = (index < count()) ? index : NoSelection;
	}


	void onMouseWheel(NAS2D::Vector<int> scrollAmount)
	{
		if (!visible() || !hasFocus() || isEmpty()) { return; }

		mScrollBar.changeValue((scrollAmount.y < 0 ? mItemSize.y : -mItemSize.y));
	}


	virtual void onSlideChange(int /*newPosition*/)
	{
		updateScrollLayout();
	}


	void onVisibilityChange(bool visible) override
	{
		Control::onVisibilityChange(visible);

		updateScrollLayout();
	}

private:
	void onMove(NAS2D::Vector<int> /*displacement*/) override
	{
		updateScrollLayout();
	}


	void onResize() override
	{
		updateScrollLayout();
	}


	void updateScrollLayout()
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

private:
	Context mContext;

	ScrollBar mScrollBar;
	NAS2D::Rectangle<int> mScrollArea;
	NAS2D::Vector<int> mItemSize;

	int mScrollOffsetInPixels = 0;
	std::size_t mHighlightIndex = NoSelection;
	std::size_t mSelectedIndex = 0;

	std::vector<ListBoxItem> mItems;
	SelectionChangedDelegate mSelectionChangedHandler;
};
