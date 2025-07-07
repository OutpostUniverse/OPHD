#include "ListBox.h"

#include <NAS2D/Resource/Font.h>
#include <NAS2D/Renderer/Renderer.h>


namespace
{
	constexpr int MarginTight{2};
}


int ListBoxItemText::ListBoxTheme::itemHeight() const
{
	return font.height() + MarginTight;
}


void ListBoxItemText::draw(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawRect, const ListBoxTheme& theme, bool /*isSelected*/, bool isHighlighted) const
{
	// Draw highlight on mouse over
	if (isHighlighted)
	{
		renderer.drawBox(drawRect, theme.itemBorderColorMouseHover);
	}

	// Draw item contents
	const auto textPosition = drawRect.position + NAS2D::Vector{MarginTight, 0};
	const auto textColor = isHighlighted ? theme.textColorMouseHover : theme.textColorNormal;
	renderer.drawTextShadow(theme.font, text, textPosition, {1, 1}, textColor, NAS2D::Color::Black);
}
