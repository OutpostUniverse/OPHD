#include "TextRender.h"

#include "../FontManager.h"
#include "../Constants.h"

#include <NAS2D/Resources/Font.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Utility.h>


void drawLabelAndValue(NAS2D::Point<int> position, const std::string& title, const std::string& text, int valuePadding, NAS2D::Color color)
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	NAS2D::Font* FONT = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	NAS2D::Font* FONT_BOLD = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);

	renderer.drawText(*FONT_BOLD, title, position, color);
	position.x() += FONT_BOLD->width(title) + valuePadding;
	renderer.drawText(*FONT, text, position, color);
}

void drawLabelAndValueLeftJustify(NAS2D::Point<int> position, const std::string& title, const std::string& text, int labelWidth, NAS2D::Color color) 
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	NAS2D::Font* FONT = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	NAS2D::Font* FONT_BOLD = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);

	renderer.drawText(*FONT_BOLD, title, position, color);
	position.x() += labelWidth;
	renderer.drawText(*FONT, text, position, color);
};