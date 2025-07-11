#include "Control.h"

#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/ResourceCache.h>

#include <stdexcept>


namespace
{
	const NAS2D::Font* defaultFont = nullptr;
	const NAS2D::Font* defaultFontBold = nullptr;
	Control::ControlImageCache* defaultImageCache = nullptr;
}


// Global static functions

void Control::setDefaultFont(const NAS2D::Font& font)
{
	defaultFont = &font;
}


void Control::setDefaultFontBold(const NAS2D::Font& font)
{
	defaultFontBold = &font;
}


void Control::setImageCache(ControlImageCache& controlImageCache)
{
	defaultImageCache = &controlImageCache;
}


const NAS2D::Font& Control::getDefaultFont()
{
	if (!defaultFont)
	{
		throw std::runtime_error("No default font set");
	}
	return *defaultFont;
}


const NAS2D::Font& Control::getDefaultFontBold()
{
	if (!defaultFontBold)
	{
		throw std::runtime_error("No default bold font set");
	}
	return *defaultFontBold;
}


const NAS2D::Image& Control::getImage(const std::string& filename)
{
	if (!defaultImageCache)
	{
		throw std::runtime_error("No default image cache set");
	}
	return defaultImageCache->load(filename);
}


// Control member functions

/**
 * Gets the rectangular area that the Control occupies.
 *
 * \return	A const reference to a Rectangle<int> object.
 */
const NAS2D::Rectangle<int>& Control::area() const
{
	return mRect;
}


void Control::area(const NAS2D::Rectangle<int>& newRect)
{
	const auto displacement = newRect.position - mRect.position;
	mRect = newRect;
	onMove(displacement);
	onResize();
}


/**
 * Sets the position of the Control.
 *
 * \param pos	2D Coordinate to position the Control at.
 */
void Control::position(NAS2D::Point<int> pos)
{
	const auto displacement = pos - mRect.position;
	mRect.startPoint(pos);
	onMove(displacement);
}


void Control::size(NAS2D::Vector<int> newSize)
{
	mRect.size = newSize;
	onResize();
}


void Control::size(int newSize)
{
	size({newSize, newSize});
}


/**
 * Sets the width of the Control.
 *
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::width(int newWidth)
{
	size({newWidth, size().y});
}


/**
 * Sets the height of the Control.
 *
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::height(int newHeight)
{
	size({size().x, newHeight});
}


/**
 * Sets highlighting of the Control.
 *
 * \param	highlight		True highlights the Control. False turns off highlighting.
 *
 * \note	Some controls may ignore this setting.
 */
void Control::highlight(bool highlight)
{
	mHighlight = highlight;
}


/**
 * Gets whether or not the Control is highlighed.
 */
bool Control::highlight() const
{
	return mHighlight;
}


/**
 * Enables/Disabled the Control.
 *
 * \param	enabled		True enables the Control. False disables the Control.
 *
 * \note	Some controls may ignore this setting.
 */
void Control::enabled(bool enabled)
{
	mEnabled = enabled;
	onEnableChange();
}


/**
 * Gets whether the Control is disabled or not.
 */
bool Control::enabled() const
{
	return mEnabled;
}


/**
 * Sets vibility of the Control.
 *
 * \param	visible	True sets Control to visible. False hides the Control.
 *
 * \note	Some controls may ignore this setting.
 */
void Control::visible(bool visible)
{
	if (mVisible != visible)
	{
		mVisible = visible;
		onVisibilityChange(mVisible);
	}
}


/**
 * Gets visibility of the Control.
 */
bool Control::visible() const
{
	return mVisible;
}


void Control::hide()
{
	visible(false);
}


void Control::show()
{
	visible(true);
}


/**
 * Sets the focus of the Control.
 */
void Control::hasFocus(bool focus)
{
	if (mHasFocus != focus)
	{
		mHasFocus = focus;
		onFocusChange();
	}
}


/**
 * Gets whether the Control is in focus or not.
 */
bool Control::hasFocus() const
{
	return mHasFocus;
}
