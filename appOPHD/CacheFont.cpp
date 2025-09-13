#include "CacheFont.h"

#include "Cache.h"
#include "Constants/UiConstants.h"


const NAS2D::Font& getFont(unsigned int size) { return fontCache.load(constants::FontPrimary, size); }
const NAS2D::Font& getFontBold(unsigned int size) { return fontCache.load(constants::FontPrimaryBold, size); }

const NAS2D::Font& getFont() { return getFont(constants::FontPrimaryNormal); }
const NAS2D::Font& getFontMedium() { return getFont(constants::FontPrimaryMedium); }

const NAS2D::Font& getFontBold() { return getFontBold(constants::FontPrimaryNormal); }
const NAS2D::Font& getFontMediumBold() { return getFontBold(constants::FontPrimaryMedium); }
const NAS2D::Font& getFontHugeBold() { return getFontBold(constants::FontPrimaryHuge); }
