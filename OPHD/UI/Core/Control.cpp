// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Control.h"

using namespace NAS2D;


/**
 * Sets the position of the Control.
 * 
 * \param pos	2D Coordinate to position the Control at.
 */
void Control::position(Point<int> pos)
{
	const auto displacement = pos - mRect.startPoint();
	mRect.startPoint(pos);
	positionChanged(displacement);
}


/**
 * Gets the X Position of the Control.
 */
int Control::positionX()
{
	return mRect.x;
}


/**
 * Gets the Y Position of the Control.
 */
int Control::positionY()
{
	return mRect.y;
}


/**
 * Callback fired whenever the Control's position changes.
 */
Control::PositionChangedCallback& Control::moved()
{
	return mPositionChanged;
}


void Control::size(NAS2D::Vector<int> newSize)
{
	mRect.size(newSize);
	onSizeChanged();
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
void Control::width(int w)
{
	mRect.width = w;
	onSizeChanged();
}


/**
 * Sets the height of the Control.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::height(int h)
{
	mRect.height = h;
	onSizeChanged();
}


Control::ResizeCallback& Control::resized()
{
	return mResized;
}


/**
 * Gets the rectangular area that the Control occupies.
 * 
 * \return	A const reference to a Rectangle<int> object.
 */
const Rectangle<int>& Control::rect() const
{
	return mRect;
}


/**
 * Sets the focus of the Control.
 */
void Control::hasFocus(bool focus)
{
	mHasFocus = focus;
	onFocusChanged();
}


/**
 * Gets whether the Control is in focus or not.
 */
bool Control::hasFocus() const
{
	return mHasFocus;
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
	enabledChanged();
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
	mVisible = visible;
	visibilityChanged(mVisible);
}


/**
 * Gets visibility of the Control.
 */
bool Control::visible() const
{
	return mVisible;
}
