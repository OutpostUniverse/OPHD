#include "TextArea.h"

#include <NAS2D/StringUtils.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>


namespace
{
	std::size_t findLastWordBreak(std::string_view lineChunk, std::size_t maxLength)
	{
		if (lineChunk.size() > maxLength)
		{
			while (maxLength > 0 && lineChunk[maxLength] != ' ')
			{
				--maxLength;
			}
		}
		return maxLength;
	}
}


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
	onLayoutText();
}


const std::string& TextArea::text() const
{
	return mText;
}


void TextArea::onResize()
{
	onLayoutText();
}


void TextArea::draw(NAS2D::Renderer& renderer) const
{
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


void TextArea::onLayoutText()
{
	mFormattedList.clear();

	const auto hardLines = NAS2D::split(mText, '\n');
	for (const auto& line : hardLines)
	{
		auto lineChunk = std::string_view{line};
		while (!lineChunk.empty())
		{
			const auto maxFitLineCharacters = mFont.widthBoundedSubstringLength(lineChunk, mRect.size.x);
			const auto lastWordBreak = findLastWordBreak(lineChunk, maxFitLineCharacters);
			const auto splitOrSliceLength = lastWordBreak > 0 ? lastWordBreak : maxFitLineCharacters;
			const auto splitLength = splitOrSliceLength > 0 ? splitOrSliceLength : 1;
			const auto softLine = lineChunk.substr(0, splitLength);
			mFormattedList.push_back(std::string{softLine});
			lineChunk = lineChunk.substr(splitLength);
		}
	}
}
