// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2020, Leeor Dicker
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#pragma once

#include "Control.h"

#include <NAS2D/Renderer/Color.h>

#include <string>


namespace NAS2D
{
	class Font;
}


class Label : public Control
{
public:
	Label(std::string newText = {});

	void text(const std::string& text);
	const std::string& text() const;

	void font(const NAS2D::Font* font);
	void color(const NAS2D::Color& color);

protected:
	void draw(NAS2D::Renderer& renderer) const override;
	void autoSize();

private:
	const NAS2D::Font* mFont;
	NAS2D::Color mTextColor{NAS2D::Color::White};
	std::string mText;
};
