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

	void textColor(const NAS2D::Color& color) { mTextColor = color; }

	void update() override;

private:
	void onResize() override;
	void onTextChange() override;

	void draw() const override;
	void processString();

	std::size_t mNumLines = 0;

	std::vector<std::string> mFormattedList;

	NAS2D::Color mTextColor = NAS2D::Color::White;

	const NAS2D::Font* mFont = nullptr;
};
