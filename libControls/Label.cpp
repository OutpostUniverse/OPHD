#include "Label.h"

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <NAS2D/Utility.h>


namespace
{
	const auto padding = NAS2D::Vector{2, 2};
}


Label::Label(std::string newText) :
	mFont{&getDefaultFont()},
	mText{newText}
{
	autoSize();
}


void Label::text(const std::string& text)
{
	mText = text;
	autoSize();
}


const std::string& Label::text() const
{
	return mText;
}


void Label::font(const NAS2D::Font* font)
{
	mFont = font;
	autoSize();
}


void Label::color(const NAS2D::Color& color)
{
	mTextColor = color;
}


void Label::draw(NAS2D::Renderer& renderer) const
{
	if (!visible()) { return; }

	const auto textPosition = mRect.position + padding;
	renderer.drawText(*mFont, mText, textPosition, mTextColor);
}


void Label::autoSize()
{
	size(mFont->size(mText) + padding * 2);
}
