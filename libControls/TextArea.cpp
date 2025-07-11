#include "TextArea.h"

#include <NAS2D/StringUtils.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>


TextArea::TextArea(NAS2D::Color textColor) :
	mFont{getDefaultFont()},
	mTextColor{textColor}
{
}


TextArea::TextArea(const NAS2D::Font& font, NAS2D::Color textColor) :
	mFont{font},
	mTextColor{textColor}
{
}


void TextArea::text(const std::string& text)
{
	mText = text;
	onTextChange();
}


const std::string& TextArea::text() const
{
	return mText;
}


void TextArea::onResize()
{
	processString();
}


void TextArea::onTextChange()
{
	processString();
}


void TextArea::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	if (highlight()) { renderer.drawBox(mRect, NAS2D::Color::White); }

	const auto displayAreaLineCount = static_cast<std::size_t>(mRect.size.y / mFont.height());
	const auto lineCount = (displayAreaLineCount < mFormattedList.size()) ? displayAreaLineCount : mFormattedList.size();
	auto textPosition = mRect.position;
	for (std::size_t i = 0; i < lineCount; ++i)
	{
		renderer.drawText(mFont, mFormattedList[i], textPosition, mTextColor);
		textPosition.y += mFont.height();
	}
}


void TextArea::processString()
{
	mFormattedList.clear();

	if (mRect.size.x < 10 || mText.empty()) { return; }

	const auto tokenList = NAS2D::split(mText, ' ');

	std::size_t i = 0;
	int w = 0;
	while (i < tokenList.size())
	{
		std::string line;
		while (w < mRect.size.x && i < tokenList.size())
		{
			int tokenWidth = mFont.width(tokenList[i] + " ");
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
}
