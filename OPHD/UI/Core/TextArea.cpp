#include "TextArea.h"

#include <NAS2D/StringUtils.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


TextArea::TextArea() :
	mFont{&getDefaultFont()}
{
}


TextArea::TextArea(const NAS2D::Font& font) :
	mFont{&font}
{
}


void TextArea::processString()
{
	mFormattedList.clear();

	if (mRect.size.x < 10 || !mFont || text().empty()) { return; }

	const auto tokenList = NAS2D::split(text(), ' ');

	std::size_t i = 0;
	int w = 0;
	while (i < tokenList.size())
	{
		std::string line;
		while (w < mRect.size.x && i < tokenList.size())
		{
			int tokenWidth = mFont->width(tokenList[i] + " ");
			w += tokenWidth;
			if (w >= mRect.size.x)
			{
				/**
				 * \todo	In some edge cases where the width of the TextArea is too
				 *			narrow for a single word/token, this will result in an infinite
				 *			loop. This edge case will need to be resolved either by splitting
				 *			the token that's too wide or by simply rendering it as is.
				 */
				//++i;
				break;
			}

			if (tokenList[i] == "\n")
			{
				++i;
				break;
			}
			line += (tokenList[i] + " ");
			++i;
		}
		w = 0;
		mFormattedList.push_back(line);
	}

	mNumLines = static_cast<std::size_t>(mRect.size.y / mFont->height());
}


void TextArea::onResize()
{
	Control::onResize();
	processString();
}


void TextArea::onTextChange()
{
	processString();
}


void TextArea::onFontChange()
{
	processString();
}


void TextArea::update()
{
	draw();
}


void TextArea::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	if (highlight()) { renderer.drawBox(mRect, NAS2D::Color::White); }

	if (!mFont) { return; }

	auto textPosition = mRect.position;
	for (std::size_t i = 0; i < mFormattedList.size() && i < mNumLines; ++i)
	{
		renderer.drawText(*mFont, mFormattedList[i], textPosition, mTextColor);
		textPosition.y += mFont->height();
	}
}
