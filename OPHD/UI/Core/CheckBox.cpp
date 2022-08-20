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

#include "../../Cache.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/MathUtils.h>

#include <algorithm>


using namespace NAS2D;


CheckBox::CheckBox(std::string newText) :
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	mSkin{imageCache.load("ui/skin/checkbox.png")}
{
	text(newText);
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &CheckBox::onMouseDown);
}


CheckBox::CheckBox(std::string newText, ClickSignal::DelegateType clickHandler) : CheckBox(newText)
{
	mSignal.connect(clickHandler);
}


CheckBox::~CheckBox()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &CheckBox::onMouseDown);
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


void CheckBox::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	onMouseDown(button, {x, y});
}


void CheckBox::onMouseDown(EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	if (button == EventHandler::MouseButton::Left && mRect.contains(position))
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
	mRect.size({std::max(mRect.width, 13), 13});
}


void CheckBox::update()
{
	if (!visible()) { return; }
	draw();
}


void CheckBox::draw() const
{
	auto& renderer = Utility<Renderer>::get();

	const auto uncheckedIconRect = NAS2D::Rectangle{0, 0, 13, 13};
	const auto checkedIconRect = NAS2D::Rectangle{13, 0, 13, 13};

	renderer.drawSubImage(mSkin, position(), (mChecked ? checkedIconRect : uncheckedIconRect));
	renderer.drawText(mFont, text(), position() + NAS2D::Vector{20, 0}, NAS2D::Color::White);
}
