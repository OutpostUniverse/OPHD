#include "Label.h"

#include "../../Constants.h"
#include "../../Cache.h"

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resources/Font.h>

#include <NAS2D/Utility.h>

static const int FIELD_PADDING = 2;
static const NAS2D::Font* TXT_FONT = nullptr;


Label::Label(std::string newText)
{
	text(newText);
	TXT_FONT = &fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	autoSize();
}


void Label::autoSize()
{
	size(textSize() + NAS2D::Vector{FIELD_PADDING * 2, FIELD_PADDING * 2});
}


void Label::font(const NAS2D::Font* font)
{
	TXT_FONT = font;
	autoSize();
}


void Label::update()
{
	if(!visible()) { return; }

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto textPosition = mRect.startPoint() + NAS2D::Vector{FIELD_PADDING, FIELD_PADDING};
	renderer.drawText(*TXT_FONT, text(), textPosition, textColor);
}


void Label::color(const NAS2D::Color& color)
{
	textColor = color;
}


NAS2D::Vector<int> Label::textSize() const
{
	return TXT_FONT->size(text());
}
