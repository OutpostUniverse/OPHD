#include "TextRender.h"

#include "../FontManager.h"
#include "../Constants.h"

#include <NAS2D/Resources/Font.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Utility.h>


void drawLabelAndValue(NAS2D::Point<int> position, const std::string& title, const std::string& text) 
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	NAS2D::Font* FONT = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	NAS2D::Font* FONT_BOLD = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);

	renderer.drawText(*FONT_BOLD, title, position, NAS2D::Color::White);
	position.x() += FONT_BOLD->width(title);
	renderer.drawText(*FONT, text, position, NAS2D::Color::White);
}
