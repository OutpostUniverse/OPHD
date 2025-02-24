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

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <algorithm>


namespace
{
	constexpr auto iconSize = NAS2D::Vector{13, 13};
	constexpr auto uncheckedIconRect = NAS2D::Rectangle{{0, 0}, iconSize};
	constexpr auto checkedIconRect = NAS2D::Rectangle{{13, 0}, iconSize};
	constexpr auto internalSpacing = 2;
	constexpr auto textOffset = NAS2D::Vector{iconSize.x + internalSpacing, 0};
}


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
	width((textWidth > 0) ? textOffset.x + textWidth : iconSize.x);
}


/**
 * Enforces minimum sizes.
 */
void CheckBox::onResize()
{
	mRect.size = {std::max(mRect.size.x, iconSize.x), std::max({mRect.size.y, iconSize.y, mFont.height()})};
}


void CheckBox::update()
{
	if (!visible()) { return; }
	draw();
}


void CheckBox::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	renderer.drawSubImage(mSkin, position() + NAS2D::Vector{0, (mRect.size.y - iconSize.y + 1) / 2}, (mChecked ? checkedIconRect : uncheckedIconRect));
	renderer.drawText(mFont, text(), position() + textOffset, NAS2D::Color::White);
}
