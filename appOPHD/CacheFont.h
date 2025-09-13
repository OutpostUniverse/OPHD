#pragma once


namespace NAS2D
{
	class Font;
}


const NAS2D::Font& getFont(unsigned int size);
const NAS2D::Font& getFontBold(unsigned int size);

const NAS2D::Font& getFont();
const NAS2D::Font& getFontMedium();

const NAS2D::Font& getFontBold();
const NAS2D::Font& getFontMediumBold();
const NAS2D::Font& getFontHugeBold();
