#pragma once

#include "Control.h"

#include "NAS2D/Renderer/Color.h"
#include "NAS2D/Resources/Font.h"

#include <string>


class TextArea : public Control
{
public:
	TextArea() = default;

	void textColor(const NAS2D::Color& color) { mTextColor = color; }

	void font(const std::string&, std::size_t);

	void update() override;

private:
	using StringList = std::vector<std::string>;

private:
	void onSizeChanged() override;
	void onTextChanged() override;
	virtual void onFontChanged();

	void draw() override;
	void processString();

private:
	std::size_t mNumLines = 0;

	StringList mFormattedList;

	NAS2D::Color mTextColor = NAS2D::Color::White;

	NAS2D::Font* mFont = nullptr;
};
