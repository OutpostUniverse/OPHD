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


	NAS2D::Color emptyAreaColor() const override
	{
		return mListBoxTheme.backgroundColorNormal;
	}


	void drawItemCell(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override
	{
		// Draw background rect
		const auto backgroundColor = (index == mSelectedIndex) ? mListBoxTheme.backgroundColorSelected : mListBoxTheme.backgroundColorNormal;
		renderer.drawBoxFilled(drawArea, backgroundColor);

		drawItem(renderer, drawArea, index);
	}


	void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override
	{
		const auto isSelected = (index == mSelectedIndex);
		const auto isHighlighted = (index == mHighlightIndex);
		mItems[index].draw(renderer, drawArea, mListBoxTheme, isSelected, isHighlighted);
	}


private:
	ListBoxTheme mListBoxTheme;
	std::vector<ListBoxItem> mItems;
};
