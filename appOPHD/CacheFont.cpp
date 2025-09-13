#include "CacheFont.h"

#include "Constants/UiConstants.h"

#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/ResourceCache.h>


namespace
{
	NAS2D::ResourceCache<NAS2D::Font, std::string, unsigned int> fontCache;
}


const NAS2D::Font& getFont(unsigned int size) { return fontCache.load(constants::FontPrimary, size); }
const NAS2D::Font& getFontBold(unsigned int size) { return fontCache.load(constants::FontPrimaryBold, size); }

const NAS2D::Font& getFont() { return getFont(constants::FontPrimaryNormal); }
const NAS2D::Font& getFontMedium() { return getFont(constants::FontPrimaryMedium); }

const NAS2D::Font& getFontBold() { return getFontBold(constants::FontPrimaryNormal); }
const NAS2D::Font& getFontMediumBold() { return getFontBold(constants::FontPrimaryMedium); }
const NAS2D::Font& getFontHugeBold() { return getFontBold(constants::FontPrimaryHuge); }
