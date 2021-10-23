#include "ListBox.h"

#include "../../Constants/UiConstants.h"

#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


unsigned int ListBoxItemText::Context::itemHeight() const
{
	return font.height() + constants::MarginTight;
}


void ListBoxItemText::draw(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, const Context& context, bool isSelected, bool isHighlighted)
{
	// Draw background rect
	const auto backgroundColor = isSelected ? context.backgroundColorSelected : context.backgroundColorNormal;
	renderer.drawBoxFilled(drawArea, backgroundColor);

	// Draw highlight on mouse over
	if (isHighlighted)
	{
		renderer.drawBox(drawArea, context.itemBorderColorMouseHover);
	}

	// Draw item contents
	const auto textPosition = drawArea.startPoint() + NAS2D::Vector{constants::MarginTight, 0};
	const auto textColor = isHighlighted ? context.textColorMouseHover : context.textColorNormal;
	renderer.drawTextShadow(context.font, text, textPosition, {1, 1}, textColor, NAS2D::Color::Black);
}
