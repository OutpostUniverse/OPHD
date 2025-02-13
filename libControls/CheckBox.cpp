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

#include <NAS2D/EventHandlerMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <algorithm>


CheckBox::CheckBox(std::string newText) :
	mFont{getDefaultFont()},
	mSkin{getImage("ui/skin/checkbox.png")}
{
	text(newText);
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &CheckBox::onMouseDown});
}


CheckBox::CheckBox(std::string newText, ClickSignal::DelegateType clickHandler) : CheckBox(newText)
{
	mSignal.connect({clickHandler});
}


CheckBox::~CheckBox()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().disconnect({this, &CheckBox::onMouseDown});
}


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


bool CheckBox::checked() const
{
	return mChecked;
}


CheckBox::ClickSignal::Source& CheckBox::click()
{
	return mSignal;
}


void CheckBox::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	if (button == NAS2D::MouseButton::Left && mRect.contains(position))
	{
		mChecked = !mChecked;
		mSignal();
	}
}


void CheckBox::onTextChange()
{
	const auto textWidth = mFont.width(text());
	width((textWidth > 0) ? 20 + textWidth : 13);
}


/**
 * Enforces minimum and maximum sizes.
 */
void CheckBox::onResize()
{
	mRect.size = {std::max(mRect.size.x, 13), 13};
}


void CheckBox::update()
{
	if (!visible()) { return; }
	draw();
}


void CheckBox::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto uncheckedIconRect = NAS2D::Rectangle<int>{{0, 0}, {13, 13}};
	const auto checkedIconRect = NAS2D::Rectangle<int>{{13, 0}, {13, 13}};

	renderer.drawSubImage(mSkin, position(), (mChecked ? checkedIconRect : uncheckedIconRect));
	renderer.drawText(mFont, text(), position() + NAS2D::Vector{20, 0}, NAS2D::Color::White);
}
