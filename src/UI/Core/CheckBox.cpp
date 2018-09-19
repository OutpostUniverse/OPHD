// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
 * ===========================================================
 * = NOTES ABOUT DECISIONS MADE
 * ===========================================================
 * = You'll note that I opted to define get/set type functions
 * = within the source file of the CheckBox class definition
 * = instead of in the header file -- this is an attempt to
 * = separate implementation details from interface details. I
 * = know, there are perhaps better ways to do this (pImpl
 * = for instance), but this makes it a bit simpler and makes
 * = the class header file a quicker read.
 * ===========================================================
 */

#include "CheckBox.h"

#include"../../Constants.h"
#include "../../FontManager.h"


using namespace NAS2D;

static Font* CBOX_FONT = nullptr;

/**
 * C'tor
 */
CheckBox::CheckBox() : mSkin("ui/skin/checkbox.png")
{
	CBOX_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, 10);
}


/**
 * D'tor
 */
CheckBox::~CheckBox()
{}


/**
 * Sets checked state.
 * 
 * \note	Setting check state with this method will not fire
 *			a clicked event.
 */
void CheckBox::checked(bool toggle)
{
	mChecked = toggle;
}


/**
 * 
 */
bool CheckBox::checked() const
{
	return mChecked;
}


/**
 * 
 */
CheckBox::ClickCallback& CheckBox::click()
{
	return mCallback;
}


/**
 * Internal function that handles hooking and unhooking events.
 */
void CheckBox::_hook_events(bool hook)
{
	if (hook)
	{
		Utility<EventHandler>::get().mouseButtonDown().connect(this, &CheckBox::onMouseDown);
	}
	else
	{
		Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &CheckBox::onMouseDown);
	}
}


/**
 * 
 */
void CheckBox::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (button == EventHandler::BUTTON_LEFT && isPointInRect(x, y, rect().x(), rect().y(), rect().width(), rect().height()))
	{
		mChecked = !mChecked;
		mCallback();
	}
}


/**
 * 
 */
void CheckBox::onFocusChanged()
{
	if (!visible()) { return; }
	_hook_events(hasFocus());
}


/**
 * 
 */
void CheckBox::onTextChanged()
{
	width(16.0f + CBOX_FONT->width(text()));
}


/**
 * Enforces minimum and maximum sizes.
 */
void CheckBox::onSizeChanged()
{
	height(clamp(height(), 14.0f, 14.0f));
	if (width() < 14.0f) { width(14.0f); }
}


/**
 * 
 */
void CheckBox::visibilityChanged(bool visible)
{
	if (!hasFocus()) { return; }
	_hook_events(visible);
}



/**
 * 
 */
void CheckBox::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawSubImage(mSkin, positionX(), positionY(), mChecked ? 14.0f : 0.0f, 0.0f, 14.0f, 14.0f);
	r.drawText(*CBOX_FONT, text(), positionX() + 20.0f, positionY(), 255, 255, 255);
}
