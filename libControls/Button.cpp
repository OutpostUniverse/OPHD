#include "Button.h"

#include "LoadRectangleSkin.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Font.h>

#include <utility>


namespace
{
	constexpr auto emptyPadding = NAS2D::Vector{2, 2};
	constexpr auto textPadding = NAS2D::Vector{8, 2};
	constexpr auto imagePadding = NAS2D::Vector{4, 4};


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


Button::Button(std::string text, ClickDelegate clickHandler) :
	Button{defaultButtonSkin(), nullptr, getDefaultFont(), std::move(text), clickHandler}
{
}


Button::Button(std::string text, NAS2D::Vector<int> initialSize, ClickDelegate clickHandler):
	Button{defaultButtonSkin(), nullptr, getDefaultFont(), std::move(text), clickHandler}
{
	size(initialSize);
}


Button::Button(const NAS2D::Image& image, ClickDelegate clickHandler) :
	Button{defaultButtonSkin(), &image, getDefaultFont(), {}, clickHandler}
{
}


Button::Button(const ButtonSkin& buttonSkin, std::string text, ClickDelegate clickHandler) :
	Button{buttonSkin, nullptr, getDefaultFont(), std::move(text), clickHandler}
{
}


Button::Button(const ButtonSkin& buttonSkin, const NAS2D::Image* image, const NAS2D::Font& font, std::string text, ClickDelegate clickHandler) :
	mButtonSkin{buttonSkin},
	mImage{image},
	mFont{&font},
	mText{std::move(text)},
	mClickHandler{clickHandler}
{
	const auto imageSize = mImage ? mImage->size() + imagePadding * 2 : NAS2D::Vector{0, 0};
	const auto textSize = !mText.empty() ? mFont->size(mText) + textPadding * 2 :
		!mImage ? NAS2D::Vector{mFont->height(), mFont->height()} + emptyPadding * 2: NAS2D::Vector{0, 0};

	const auto defaultSize = NAS2D::Vector{
		(imageSize.x > textSize.x) ? imageSize.x : textSize.x,
		(imageSize.y > textSize.y) ? imageSize.y : textSize.y,
	};

	size(defaultSize);

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


void Button::draw(NAS2D::Renderer& renderer) const
{
	if (!visible()) { return; }

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
