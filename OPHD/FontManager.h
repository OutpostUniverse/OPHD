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
class FontManager
{
public:
	/**
	 * C'tor
	 */
	FontManager()
	{}
	
	/**
	 * D'tor
	 */
	~FontManager()
	{
	}

	/**
	 * Gets a pointer to a NAS2D::Font object given a filename on disk
	 * and a size in points.
	 * 
	 * \param	name	Name of the font. This matches the filename on disk.
	 * \param	size	Size in point (pt).
	 * 
	 * \note	In practice for the sake of efficiency a reference to the NAS2D::Font should
	 *			be stored rather than calling font() repeatedly. This avoids unnecessary
	 *			table lookups.
	 * 
	 * \warning	The pointer returned by font() is owned by FontManager. Do not dispose of the
	 *			pointer manually.
	 */
	const NAS2D::Font* font(const std::string& name, int size)
	{
		return &mFontTable.load(name, static_cast<unsigned int>(size));
	}

private:
	ResourceCache<NAS2D::Font, std::string, unsigned int> mFontTable;
};
