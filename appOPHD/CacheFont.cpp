#include "CacheFont.h"

#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/ResourceCache.h>


namespace
{
	const std::string FontPrimary = "fonts/opensans.ttf";
	const std::string FontPrimaryBold = "fonts/opensans-bold.ttf";

	constexpr int FontPrimaryNormal{14};
	constexpr int FontPrimaryMedium{18};
	constexpr int FontPrimaryHuge{24};

	NAS2D::ResourceCache<NAS2D::Font, std::string, unsigned int> fontCache;
}


const NAS2D::Font& getFont(unsigned int size) { return fontCache.load(FontPrimary, size); }
const NAS2D::Font& getFontBold(unsigned int size) { return fontCache.load(FontPrimaryBold, size); }

const NAS2D::Font& getFont() { return getFont(FontPrimaryNormal); }
const NAS2D::Font& getFontMedium() { return getFont(FontPrimaryMedium); }

const NAS2D::Font& getFontBold() { return getFontBold(FontPrimaryNormal); }
const NAS2D::Font& getFontMediumBold() { return getFontBold(FontPrimaryMedium); }
const NAS2D::Font& getFontHugeBold() { return getFontBold(FontPrimaryHuge); }
