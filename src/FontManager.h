#pragma once


#include <NAS2D/NAS2D.h>

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
		for (auto font : mFontTable) { delete font.second; }
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
	NAS2D::Font* font(const std::string& name, size_t size)
	{
		auto it = mFontTable.find(FontId(name, size));
		if (it != mFontTable.end())
		{
			return it->second;
		}
		else
		{
			NAS2D::Font* new_font = new NAS2D::Font(name, static_cast<int>(size));
			mFontTable[FontId(name, size)] = new_font;
			return new_font;
		}
	}

private:
	typedef std::pair<std::string, size_t> FontId;
	typedef std::map<FontId, NAS2D::Font*> FontTable;

private:
	FontTable	mFontTable;
};
