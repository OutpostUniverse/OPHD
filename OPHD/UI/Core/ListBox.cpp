#include "ListBox.h"

#include <NAS2D/Renderer/Renderer.h>


namespace
{
	constexpr int MarginTight{2};
}


unsigned int ListBoxItemText::Context::itemHeight() const
{
	return static_cast<unsigned int>(font.height() + MarginTight);
}


void ListBoxItemText::draw(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawRect, const Context& context, bool isSelected, bool isHighlighted) const
{
	// Draw background rect
	const auto backgroundColor = isSelected ? context.backgroundColorSelected : context.backgroundColorNormal;
	renderer.drawBoxFilled(drawRect, backgroundColor);

	// Draw highlight on mouse over
	if (isHighlighted)
	{
		renderer.drawBox(drawRect, context.itemBorderColorMouseHover);
	}

	// Draw item contents
	const auto textPosition = drawRect.position + NAS2D::Vector{MarginTight, 0};
	const auto textColor = isHighlighted ? context.textColorMouseHover : context.textColorNormal;
	renderer.drawTextShadow(context.font, text, textPosition, {1, 1}, textColor, NAS2D::Color::Black);
}
