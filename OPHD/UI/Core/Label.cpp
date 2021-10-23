#include "Label.h"

#include "../../Cache.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <NAS2D/Utility.h>


Label::Label(std::string newText) :
	mFont(&fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal))
{
	text(newText);
	autosize();
}


void Label::autosize()
{
	size(textSize() + NAS2D::Vector{mPadding * 2, mPadding * 2});
}


void Label::font(const NAS2D::Font* font)
{
	mFont = font;
	autosize();
}


void Label::update()
{
	if (!visible()) { return; }

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto textPosition = mRect.startPoint() + NAS2D::Vector{mPadding, mPadding};
	renderer.drawText(*mFont, text(), textPosition, mTextColor);
}

int Label::textWidth() const
{
	return mFont->width(text());
}

NAS2D::Vector<int> Label::textSize() const
{
	return mFont->size(text());
}

void Label::color(const NAS2D::Color& color)
{
	mTextColor = color;
}
