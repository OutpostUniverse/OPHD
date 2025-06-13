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

	void text(const std::string& text);
	const std::string& text() const;

	void font(const NAS2D::Font* font);
	void color(const NAS2D::Color& color);

protected:
	void draw() const override;
	void autoSize();

private:
	NAS2D::Color mTextColor{NAS2D::Color::White};
	const NAS2D::Font* mFont;
	const int mPadding = 2;
};
