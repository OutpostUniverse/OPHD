// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "RadioButton.h"

#include"../../Constants.h"
#include "../../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"
#include "NAS2D/MathUtils.h"

#include "UIContainer.h"

#include <algorithm>


using namespace NAS2D;

static Font* CBOX_FONT = nullptr;

/**
 * C'tor
 */
RadioButton::RadioButton(std::string newText) :
	mSkin("ui/skin/checkbox.png"),
	mLabel(newText)
{
	CBOX_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &RadioButton::onMouseDown);
}

RadioButton::~RadioButton()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &RadioButton::onMouseDown);
}

/**
 * Sets checked state.
 *
 * \note	Setting check state with this method will not fire
 *			a clicked event.
 */
void RadioButton::checked(bool toggle)
{
	mChecked = toggle;
}


/**
 *
 */
bool RadioButton::checked() const
{
	return mChecked;
}

void RadioButton::parentContainer(UIContainer* parent)
{
	mParentContainer = parent;
}

void RadioButton::text(const std::string& text)
{
	mLabel.text(text);
	width(20.0f + CBOX_FONT->width(text));
	onTextChanged();
}

const std::string& RadioButton::text() const
{
	return mLabel.text();
}

/**
 *
 */
RadioButton::ClickCallback& RadioButton::click()
{
	for (auto* sibling : mParentContainer->controls())
	{
		if (auto* asRadioButton = dynamic_cast<RadioButton*>(sibling))
		{
			asRadioButton->checked(false);
		}
	}
	checked(true);
	return mCallback;
}


/**
 *
 */
void RadioButton::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!enabled() || !visible() || !hasFocus()) { return; }

	if (button == EventHandler::MouseButton::BUTTON_LEFT && mRect.to<int>().contains(Point{x, y}))
	{
		click();
		mCallback();
	}
}


/**
 *
 */
void RadioButton::onTextChanged()
{
	width(16.0f + CBOX_FONT->width(text()));
}


/**
 * Enforces minimum and maximum sizes.
 */
void RadioButton::onSizeChanged()
{
	mRect.height(std::clamp(height(), 13.0f, 13.0f));
	if (width() < 13.0f) { mRect.width(13.0f); }
}


/**
 *
 */
void RadioButton::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawSubImage(mSkin, positionX(), positionY(), mChecked ? 13.0f : 0.0f, 0.0f, 13.0f, 13.0f);
	r.drawText(*CBOX_FONT, text(), positionX() + 20.0f, positionY(), 255, 255, 255);
}
