// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2020, Leeor Dicker
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#pragma once

#include "TextControl.h"

#include <NAS2D/Renderer/Color.h>

#include <string>


namespace NAS2D
{
	class Font;
}


/**
 * A control that contains readonly text.
 */
class Label : public TextControl
{
public:
	Label(std::string newText = {});

	void autosize();
	void font(const NAS2D::Font* font);
	bool isEmpty() const { return text().empty(); }
	void clear() { mText.clear(); }

	void update() override;
	void draw() const override;

	int textWidth() const;
	NAS2D::Vector<int> textSize() const;

	void color(const NAS2D::Color& color);

private:
	NAS2D::Color mTextColor{NAS2D::Color::White};
	const NAS2D::Font* mFont;
	const int mPadding = 2;
};
