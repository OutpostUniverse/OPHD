#include "Label.h"

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <NAS2D/Utility.h>


Label::Label(std::string newText) :
	mFont{&getDefaultFont()}
{
	text(newText);
	autoSize();
}


void Label::text(const std::string& text)
{
	TextControl::text(text);
	autoSize();
}


const std::string& Label::text() const
{
	return TextControl::text();
}


void Label::font(const NAS2D::Font* font)
{
	mFont = font;
	autoSize();
}


void Label::update()
{
	if (!visible()) { return; }
	draw();
}


void Label::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto textPosition = mRect.position + NAS2D::Vector{mPadding, mPadding};
	renderer.drawText(*mFont, text(), textPosition, mTextColor);
}


NAS2D::Vector<int> Label::textSize() const
{
	return mFont->size(text());
}


void Label::color(const NAS2D::Color& color)
{
	mTextColor = color;
}


void Label::autoSize()
{
	size(textSize() + NAS2D::Vector{mPadding, mPadding} * 2);
}
