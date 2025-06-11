#pragma once

#include "TextControl.h"

#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Resource/Font.h>

#include <string>


class TextArea : public TextControl
{
public:
	TextArea();
	TextArea(const NAS2D::Font& font);

	void textColor(NAS2D::Color color) { mTextColor = color; }

	void update() override;

protected:
	void onResize() override;
	void onTextChange() override;

	void draw() const override;
	void processString();

private:
	std::vector<std::string> mFormattedList;

	NAS2D::Color mTextColor = NAS2D::Color::White;

	const NAS2D::Font& mFont;
};
