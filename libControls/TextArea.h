#pragma once

#include "Control.h"

#include <NAS2D/Renderer/Color.h>

#include <string>
#include <vector>


namespace NAS2D
{
	class Font;
}


class TextArea : public Control
{
public:
	TextArea(NAS2D::Color textColor = NAS2D::Color::White);
	TextArea(const NAS2D::Font& font, NAS2D::Color textColor = NAS2D::Color::White);

	void text(const std::string& text);
	const std::string& text() const;

	void draw() const override;

protected:
	void onResize() override;
	void onLayoutText();

private:
	const NAS2D::Font& mFont;
	const NAS2D::Color mTextColor;
	std::string mText;
	std::vector<std::string> mFormattedList;
};
