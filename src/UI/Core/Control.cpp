#include "Control.h"


/**
 * C'tor
 */
Control::Control():		mFont(NULL),
						mEnabled(true),
						mHasFocus(false),
						mVisible(true),
						mHighlight(false)
{}


/**
 * D'tor
 */
Control::~Control()
{}


/**
 * Sets the internal font.
 *
 * \param	font		A reference to a Font object.
 * 
 * \warning	Control stores the set font in a pointer. This pointer is NOT owned
 *			by Control. The Font passed to Control should not be allowed to go
 *			out of scope until the Control is destroyed.
 */
void Control::font(Font& font)
{
	mFont = &font;

	onFontChanged();
}


/**
 * Internal function to provide access to the Font object
 * to derived objects.
 */
Font& Control::font()
{
	return *mFont;
}


/**
 * Internal funtion used to determine if a Font is available
 * for use.
 */
bool Control::fontSet() const
{
	return mFont != NULL;
}


/**
 * Sets the position of the Control.
 * 
 * \param pos	2D Coordinate to position the Control at.
 */
void Control::position(const Point_2d& pos)
{
	position(static_cast<float>(pos.x()), static_cast<float>(pos.y()));
}


/**
 * Sets the position of the Control.
 * 
 * \param pos	2D Coordinate to position the Control at.
 */
void Control::position(const Point_2df& pos)
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
	mRect.x(x);
	mRect.y(y);
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
 * Gets the width of the Control
 */
float Control::width() const
{
	return mRect.w();
}


/**
 * Gets the height of the Control.
 */
float Control::height() const
{
	return mRect.h();
}


/**
 * Sets the Width and Height of the Control.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::size(float w, float h)
{
	width(w);
	height(h);
}


/**
 * Sets the width of the Control.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::width(float w)
{
	mRect.w(w);
}


/**
 * Sets the height of the Control.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
void Control::height(float h)
{
	mRect.h(h);
}


/**
 * Gets the rectangular area that the Control occupies.
 * 
 * \return	A const reference to a Rectangle_2d object.
 */
const Rectangle_2df& Control::rect() const
{
	return mRect;
}


/**
 * Sets the focus of the Control.
 */
void Control::hasFocus(bool focus)
{
	mHasFocus = focus;
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


/**
 * Gets the text of the Control.
 */
const string& Control::text() const
{
	return mText;
}


/**
 * Internal version of the rect() function which allows
 * non-const access to the rectangle information.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
Rectangle_2df& Control::_rect()
{
	return mRect;
}


/**
 * Internal version of the text() function which allows
 * non-const access to the text contained in the Control.
 * 
 * \note	This is an internal function and may not be
 *			called outside of the Control class.
 */
string& Control::_text()
{
	return mText;
}