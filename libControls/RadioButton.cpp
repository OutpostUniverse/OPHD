#include "RadioButtonGroup.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <algorithm>


namespace
{
	constexpr auto iconSize = NAS2D::Vector{10, 10};
	constexpr auto unselectedIconRect = NAS2D::Rectangle{{1, 1}, iconSize};
	constexpr auto selectedIconRect = NAS2D::Rectangle{{14, 1}, iconSize};
	constexpr auto internalSpacing = 2;
	constexpr auto textOffset = NAS2D::Vector{iconSize.x + internalSpacing, 0};
}


RadioButtonGroup::RadioButton::RadioButton(RadioButtonGroup& parentContainer, std::string newText) :
	mFont{getDefaultFont()},
	mSkin{getImage("ui/skin/radio.png")},
	mLabel{newText},
	mParentContainer{parentContainer}
{
	text(newText);
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &RadioButtonGroup::RadioButton::onMouseDown});
}


RadioButtonGroup::RadioButton::~RadioButton()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().disconnect({this, &RadioButtonGroup::RadioButton::onMouseDown});
}


/**
 * Sets checked state.
 *
 * \note	Setting check state with this method will not fire
 *			a clicked event.
 */
void RadioButtonGroup::RadioButton::checked(bool toggle)
{
	mChecked = toggle;
}


bool RadioButtonGroup::RadioButton::checked() const
{
	return mChecked;
}


void RadioButtonGroup::RadioButton::text(const std::string& text)
{
	mLabel.text(text);
	onTextChange();
}


const std::string& RadioButtonGroup::RadioButton::text() const
{
	return mLabel.text();
}


void RadioButtonGroup::RadioButton::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto iconPosition = position() + NAS2D::Vector{0, (mRect.size.y - iconSize.y) / 2};
	renderer.drawSubImage(mSkin, iconPosition, (mChecked ? selectedIconRect : unselectedIconRect));
	renderer.drawText(mFont, text(), position() + textOffset, NAS2D::Color::White);
}


/**
 * Enforces minimum sizes.
 */
void RadioButtonGroup::RadioButton::onResize()
{
	mRect.size = {std::max(mRect.size.x, iconSize.x), std::max({iconSize.y, mFont.height()})};
}


void RadioButtonGroup::RadioButton::onTextChange()
{
	const auto textWidth = mFont.width(text());
	width((textWidth > 0) ? textOffset.x + textWidth : iconSize.x);
}


void RadioButtonGroup::RadioButton::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible() || !mRect.contains(position)) { return; }

	if (button == NAS2D::MouseButton::Left)
	{
		mParentContainer.select(*this);
	}
}
