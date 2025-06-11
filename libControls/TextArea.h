#pragma once

#include "TextControl.h"

#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Resource/Font.h>

#include <string>


class TextArea : public TextControl
{
public:
	TextArea(NAS2D::Color textColor = NAS2D::Color::White);
	TextArea(const NAS2D::Font& font, NAS2D::Color textColor = NAS2D::Color::White);

	void textColor(NAS2D::Color color) { mTextColor = color; }

	void update() override;

protected:
	void onResize() override;
	void onTextChange() override;

	void draw() const override;
	void processString();

private:
	const NAS2D::Font& mFont;
	NAS2D::Color mTextColor = NAS2D::Color::White;
	std::vector<std::string> mFormattedList;
};
