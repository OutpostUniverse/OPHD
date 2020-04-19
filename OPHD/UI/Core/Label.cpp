#include "Label.h"

#include "../../Constants.h"
#include "../../FontManager.h"

#include "NAS2D/Renderer/Renderer.h"
#include "NAS2D/Resources/Font.h"

#include "NAS2D/Utility.h"

static const int FIELD_PADDING = 4;
static NAS2D::Font* TXT_FONT = nullptr;


Label::Label(std::string newText)
{
	text(newText);
	TXT_FONT = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	height(static_cast<float>(TXT_FONT->height() + FIELD_PADDING * 2));
}


void Label::font(NAS2D::Font* font)
{
	TXT_FONT = font;
	height(static_cast<float>(TXT_FONT->height() + FIELD_PADDING * 2));
}


void Label::update()
{
	if(!visible()) { return; }

	NAS2D::Renderer& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto textPosition = mRect.startPoint().to<int>();
	renderer.drawText(*TXT_FONT, text(), textPosition + NAS2D::Vector{FIELD_PADDING, FIELD_PADDING}, textColor);
}


void Label::color(const NAS2D::Color& color)
{
	textColor = color;
}


int Label::textWidth() const
{
	return TXT_FONT->width(text());
}

NAS2D::Vector<int> Label::textSize() const
{
	return {TXT_FONT->width(text()), TXT_FONT->height()};
}
