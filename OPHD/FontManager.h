#pragma once


#include <NAS2D/Resources/Font.h>
#include <NAS2D/Resources/ResourceCache.h>


/**
 * A basic font table lookup manager for NAS2D::Font objects.
 * 
 * FontManager implements a basic font manager objects. Fonts are asked for by filename and by
 * point size. Any time a font is requested and is found on disk, it is added to the FontManager's
 * font table.
 * 
 * The FontManager class is intended to be invoked with the NAS2D::Utility object so as to maintain
 * scope throughout the lifetime of a NAS2D application.
 */
using FontManager = ResourceCache<NAS2D::Font, std::string, unsigned int>;
