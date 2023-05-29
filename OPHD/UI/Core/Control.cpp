#include "Control.h"


void Control::area(const NAS2D::Rectangle<int>& newRect)
{
	const auto displacement = newRect.startPoint() - mRect.startPoint();
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
	const auto displacement = pos - mRect.startPoint();
	mRect.startPoint(pos);
	onMove(displacement);
}


/**
 * Gets the X Position of the Control.
 */
int Control::positionX() const
{
	return mRect.startPoint().x;
}


/**
 * Gets the Y Position of the Control.
 */
int Control::positionY() const
{
	return mRect.startPoint().y;
}


/**
 * Signal fired whenever the Control's position changes.
 */
Control::OnMoveSignal::Source& Control::moved()
{
	return mOnMoveSignal;
}


void Control::size(NAS2D::Vector<int> newSize)
{
	mRect.size(newSize);
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
void Control::width(int w)
{
	size({w, size().y});
}


/**
 * Sets the height of the Control.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::height(int h)
{
	size({size().x, h});
}


Control::ResizeSignal::Source& Control::resized()
{
	return mOnResizeSignal;
}


/**
 * Gets the rectangular area that the Control occupies.
 * 
 * \return	A const reference to a Rectangle<int> object.
 */
const NAS2D::Rectangle<int>& Control::rect() const
{
	return mRect;
}


/**
 * Sets the focus of the Control.
 */
void Control::hasFocus(bool focus)
{
	mHasFocus = focus;
	onFocusChange();
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
	mVisible = visible;
	onVisibilityChange(mVisible);
}


/**
 * Gets visibility of the Control.
 */
bool Control::visible() const
{
	return mVisible;
}
