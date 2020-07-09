// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Control.h"

using namespace NAS2D;


/**
 * Sets the position of the Control.
 * 
 * \param pos	2D Coordinate to position the Control at.
 */
void Control::position(const Point<float>& pos)
{
	position(pos.x(), pos.y());
}


/**
 * Sets the position of the Control.
 * 
 * \param x		X-Coordinate to position the Control at.
 * \param x		Y-Coordinate to position the Control at.
 */
void Control::position(float x, float y)
{
	float dx = x - mRect.x(), dy = y - mRect.y();

	mRect.x(x);
	mRect.y(y);

	positionChanged(dx, dy);
}


/**
 * Gets the X Position of the Control.
 */
float Control::positionX()
{
	return mRect.x();
}


/**
 * Gets the Y Position of the Control.
 */
float Control::positionY()
{
	return mRect.y();
}


/**
 * Callback fired whenever the Control's position changes.
 */
Control::PositionChangedCallback& Control::moved()
{
	return mPositionChanged;
}


/**
 * Gets the width of the Control
 */
float Control::width() const
{
	return mRect.width();
}


/**
 * Gets the height of the Control.
 */
float Control::height() const
{
	return mRect.height();
}


void Control::size(NAS2D::Vector<float> newSize)
{
	mRect.width(newSize.x);
	mRect.height(newSize.y);
	onSizeChanged();
}


void Control::size(float newSize)
{
	size({newSize, newSize});
}


/**
 * Sets the width of the Control.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::width(float w)
{
	mRect.width(w);
	onSizeChanged();
}


/**
 * Sets the height of the Control.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::height(float h)
{
	mRect.height(h);
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
const Rectangle<float>& Control::rect() const
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
 * \param	enabled		True highlights the Control. False turns off highlighting.
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


/**
 * Sets the text of the Control.
 */
void Control::text(const std::string& text)
{
	mText = text;
	onTextChanged();
}
