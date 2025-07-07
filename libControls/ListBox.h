#pragma once

#include "ListBoxBase.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Renderer/Renderer.h>

#include <string>
#include <vector>
#include <utility>
#include <stdexcept>


namespace NAS2D
{
	class Font;
}


struct ListBoxItemText
{
	std::string text;
	int userData = 0;

	struct ListBoxTheme
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

	void draw(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> itemDrawRect, const ListBoxTheme& theme, bool isSelected, bool isHighlighted) const;

	bool operator<(const ListBoxItemText& other) const
	{
		return text < other.text;
	}
};


template <typename ListBoxItem = ListBoxItemText>
class ListBox : public ListBoxBase
{
public:
	using ListBoxTheme = typename ListBoxItem::ListBoxTheme;


	ListBox(SelectionChangedDelegate selectionChangedHandler = {}) :
		ListBox{{getDefaultFont()}, selectionChangedHandler}
	{
	}


	ListBox(ListBoxTheme theme, SelectionChangedDelegate selectionChangedHandler = {}) :
		ListBoxBase{{0, theme.itemHeight()}, selectionChangedHandler},
		mListBoxTheme{theme}
	{
		updateScrollLayout();
	}


	template <typename... Args>
	void add(Args&&... args)
	{
		mItems.emplace_back(ListBoxItem{std::forward<Args>(args)...});
		updateScrollLayout();
	}


	const ListBoxItem& selected() const
	{
		if (mSelectedIndex == NoSelection)
		{
			throw std::runtime_error("ListBox has no selected item");
		}

		return mItems[mSelectedIndex];
	}


	template <typename UnaryPredicate>
	void selectIf(UnaryPredicate predicate)
	{
		for (std::size_t i = 0; i < count(); ++i)
		{
			if (predicate(mItems[i]))
			{
				selectedIndex(i);
				return;
			}
		}
	}


	std::size_t count() const override
	{
		return mItems.size();
	}


	void clear()
	{
		mItems.clear();
		ListBoxBase::clear();
	}


	int itemHeight() const
	{
		return mItemSize.y;
	}


protected:
	NAS2D::Color borderColor() const override
	{
		return hasFocus() ? mListBoxTheme.borderColorActive : mListBoxTheme.borderColorNormal;
	}


	void draw() const override
	{
		auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

		renderer.drawBox(mRect, borderColor());

		drawScrollArea(renderer);
	}


	void drawScrollArea(NAS2D::Renderer& renderer) const
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
			drawItem(renderer, itemDrawArea, index);
			itemDrawArea.position.y += lineHeight;
		}

		// Paint remaining section of scroll area not covered by items
		itemDrawArea.size.y = mScrollArea.endPoint().y - itemDrawArea.position.y;
		renderer.drawBoxFilled(itemDrawArea, mListBoxTheme.backgroundColorNormal);

		renderer.clipRectClear();
	}


	void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override
	{
		const auto isSelected = (index == mSelectedIndex);
		const auto isHighlighted = (index == mHighlightIndex);

		// Draw background rect
		const auto backgroundColor = isSelected ? mListBoxTheme.backgroundColorSelected : mListBoxTheme.backgroundColorNormal;
		renderer.drawBoxFilled(drawArea, backgroundColor);

		mItems[index].draw(renderer, drawArea, mListBoxTheme, isSelected, isHighlighted);
	}


	void onMove(NAS2D::Vector<int> /*displacement*/) override
	{
		updateScrollLayout();
	}

private:
	ListBoxTheme mListBoxTheme;
	std::vector<ListBoxItem> mItems;
};
