#include "Button.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


namespace
{
	constexpr auto internalPadding = NAS2D::Vector{2, 2};
}


Button::Button(std::string newText) :
	mButtonSkin{
		{
			getImage("ui/skin/button_top_left.png"),
			getImage("ui/skin/button_top_middle.png"),
			getImage("ui/skin/button_top_right.png"),
			getImage("ui/skin/button_middle_left.png"),
			getImage("ui/skin/button_middle_middle.png"),
			getImage("ui/skin/button_middle_right.png"),
			getImage("ui/skin/button_bottom_left.png"),
			getImage("ui/skin/button_bottom_middle.png"),
			getImage("ui/skin/button_bottom_right.png")
		},
		{
			getImage("ui/skin/button_hover_top_left.png"),
			getImage("ui/skin/button_hover_top_middle.png"),
			getImage("ui/skin/button_hover_top_right.png"),
			getImage("ui/skin/button_hover_middle_left.png"),
			getImage("ui/skin/button_hover_middle_middle.png"),
			getImage("ui/skin/button_hover_middle_right.png"),
			getImage("ui/skin/button_hover_bottom_left.png"),
			getImage("ui/skin/button_hover_bottom_middle.png"),
			getImage("ui/skin/button_hover_bottom_right.png")
		},
		{
			getImage("ui/skin/button_pressed_top_left.png"),
			getImage("ui/skin/button_pressed_top_middle.png"),
			getImage("ui/skin/button_pressed_top_right.png"),
			getImage("ui/skin/button_pressed_middle_left.png"),
			getImage("ui/skin/button_pressed_middle_middle.png"),
			getImage("ui/skin/button_pressed_middle_right.png"),
			getImage("ui/skin/button_pressed_bottom_left.png"),
			getImage("ui/skin/button_pressed_bottom_middle.png"),
			getImage("ui/skin/button_pressed_bottom_right.png")
		}
	}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &Button::onMouseDown});
	eventHandler.mouseButtonUp().connect({this, &Button::onMouseUp});
	eventHandler.mouseMotion().connect({this, &Button::onMouseMove});

	mFont = &getDefaultFont();
	text(newText);
	size(mFont->size(text()) + internalPadding * 2);
}


Button::Button(std::string newText, ClickSignal::DelegateType clickHandler) : Button(newText)
{
	mSignal.connect({clickHandler});
}


Button::Button(std::string text, NAS2D::Vector<int> sz, ClickSignal::DelegateType clickHandler):
	Button(text, clickHandler)
{
	size(sz);
}


Button::Button(const ButtonSkin& buttonSkin, ClickSignal::DelegateType clickHandler) :
	mButtonSkin{buttonSkin}
{
	mSignal.connect({clickHandler});
}


Button::~Button()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &Button::onMouseDown});
	eventHandler.mouseButtonUp().disconnect({this, &Button::onMouseUp});
	eventHandler.mouseMotion().disconnect({this, &Button::onMouseMove});
}


void Button::type(Type type)
{
	mType = type;
}


void Button::toggle(bool toggle)
{
	mIsPressed = toggle;
}


bool Button::isPressed() const
{
	return mIsPressed;
}


void Button::font(const NAS2D::Font& font)
{
	mFont = &font;
}


void Button::image(const std::string& path)
{
	mImage = &getImage(path);
}


bool Button::hasImage() const
{
	return mImage != nullptr;
}


void Button::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible() || !mRect.contains(position)) { return; }
	if (button != NAS2D::MouseButton::Left) { return; }

			if (mType == Type::Push)
			{
				mIsPressed = true;
			}
			else
			{
				mIsPressed = !mIsPressed;
				mSignal();
			}
}


void Button::onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	if (button == NAS2D::MouseButton::Left)
	{
		if (mType == Type::Push)
		{
			mIsPressed = false;

			if (mRect.contains(position))
			{
				mSignal();
			}
		}
	}
}


void Button::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	mMouseHover = mRect.contains(position);
}


void Button::update()
{
	draw();
}


/**
 * Draws the button.
 */
void Button::draw() const
{
	if (!visible()) { return; }

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto& skin = (mIsPressed) ? mButtonSkin.pressed :
		(enabled() && mMouseHover) ? mButtonSkin.hover : mButtonSkin.normal;
	skin.draw(renderer, mRect);

	if (mImage)
	{
		renderer.drawImage(*mImage, mRect.center() - mImage->size() / 2);
	}
	else
	{
		const auto textPosition = mRect.center() - mFont->size(text()) / 2;
		renderer.drawText(*mFont, text(), textPosition, NAS2D::Color::White);
	}

	/// \fixme	Naive... would rather set a b&w shader instead.
	if (!enabled())
	{
		renderer.drawBoxFilled(mRect, NAS2D::Color{125, 125, 125, 100});
	}
}


