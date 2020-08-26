#include "Label.h"

#include "../../Constants.h"
#include "../../Cache.h"

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resources/Font.h>

#include <NAS2D/Utility.h>

static const int FIELD_PADDING = 2;


Label::Label(std::string newText) :
	mFont(&fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL))
{
	text(newText);
	resize();
}


void Label::resize()
{
	size(mFont->size(text()) + NAS2D::Vector{FIELD_PADDING * 2, FIELD_PADDING * 2});
}


void Label::font(const NAS2D::Font* font)
{
	mFont = font;
	resize();
}


void Label::update()
{
	if(!visible()) { return; }

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto textPosition = mRect.startPoint() + NAS2D::Vector{FIELD_PADDING, FIELD_PADDING};
	renderer.drawText(*mFont, text(), textPosition, textColor);
}


void Label::color(const NAS2D::Color& color)
{
	textColor = color;
}

