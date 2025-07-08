#include "Button.h"

#include "LoadRectangleSkin.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Font.h>


namespace
{
	constexpr auto internalPadding = NAS2D::Vector{2, 2};


	const Button::ButtonSkin& defaultButtonSkin()
	{
		static const Button::ButtonSkin defaultButtonSkin{
			loadRectangleSkin("ui/skin/button_normal"),
			loadRectangleSkin("ui/skin/button_hover"),
			loadRectangleSkin("ui/skin/button_pressed"),
		};
		return defaultButtonSkin;
	}
}


Button::Button(std::string newText) :
	mButtonSkin{defaultButtonSkin()},
	mFont{&getDefaultFont()},
	mText{newText}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &Button::onMouseDown});
	eventHandler.mouseButtonUp().connect({this, &Button::onMouseUp});
	eventHandler.mouseMotion().connect({this, &Button::onMouseMove});

	size(mFont->size(mText) + internalPadding * 2);
}


Button::Button(std::string newText, ClickDelegate clickHandler) :
	Button(newText)
{
	mClickHandler = clickHandler;
}


Button::Button(std::string text, NAS2D::Vector<int> sz, ClickDelegate clickHandler):
	Button(text, clickHandler)
{
	size(sz);
}


Button::Button(const NAS2D::Image& image, ClickDelegate clickHandler) :
	Button()
{
	mImage = &image;
	size(mImage->size() + internalPadding * 2);
	mClickHandler = clickHandler;
}


Button::Button(const ButtonSkin& buttonSkin, ClickDelegate clickHandler) :
	Button{buttonSkin, nullptr, getDefaultFont(), {}, clickHandler}
{
}


Button::Button(const ButtonSkin& buttonSkin, const NAS2D::Image* image, const NAS2D::Font& font, std::string newText, ClickDelegate clickHandler) :
	mButtonSkin{buttonSkin},
	mImage{image},
	mFont{&font},
	mText{newText},
	mClickHandler{clickHandler}
{
	size(mFont->size(mText) + internalPadding * 2);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &Button::onMouseDown});
	eventHandler.mouseButtonUp().connect({this, &Button::onMouseUp});
	eventHandler.mouseMotion().connect({this, &Button::onMouseMove});
}


Button::~Button()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &Button::onMouseDown});
	eventHandler.mouseButtonUp().disconnect({this, &Button::onMouseUp});
	eventHandler.mouseMotion().disconnect({this, &Button::onMouseMove});
}


void Button::click() const
{
	if (mClickHandler) { mClickHandler(); }
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


void Button::text(const std::string& text)
{
	mText = text;
}


const std::string& Button::text() const
{
	return mText;
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
		click();
	}
}


void Button::onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }
	if (button != NAS2D::MouseButton::Left) { return; }

	if (mType == Type::Push)
	{
		mIsPressed = false;

		if (mRect.contains(position))
		{
			click();
		}
	}
}


void Button::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	mMouseHover = mRect.contains(position);
}


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
	if (mFont)
	{
		const auto textPosition = mRect.center() - mFont->size(mText) / 2;
		renderer.drawText(*mFont, mText, textPosition, NAS2D::Color::White);
	}

	if (!enabled())
	{
		renderer.drawBoxFilled(mRect, NAS2D::Color{125, 125, 125, 100});
	}
}


