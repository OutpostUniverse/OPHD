#include "Button.h"

#include "../../Cache.h"
#include "../../Common.h"
#include "../../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/MathUtils.h>


using namespace NAS2D;


Button::Button(std::string newText) :
	mSkinNormal{
		imageCache.load("ui/skin/button_top_left.png"),
		imageCache.load("ui/skin/button_top_middle.png"),
		imageCache.load("ui/skin/button_top_right.png"),
		imageCache.load("ui/skin/button_middle_left.png"),
		imageCache.load("ui/skin/button_middle_middle.png"),
		imageCache.load("ui/skin/button_middle_right.png"),
		imageCache.load("ui/skin/button_bottom_left.png"),
		imageCache.load("ui/skin/button_bottom_middle.png"),
		imageCache.load("ui/skin/button_bottom_right.png")
	},
	mSkinHover{
		imageCache.load("ui/skin/button_hover_top_left.png"),
		imageCache.load("ui/skin/button_hover_top_middle.png"),
		imageCache.load("ui/skin/button_hover_top_right.png"),
		imageCache.load("ui/skin/button_hover_middle_left.png"),
		imageCache.load("ui/skin/button_hover_middle_middle.png"),
		imageCache.load("ui/skin/button_hover_middle_right.png"),
		imageCache.load("ui/skin/button_hover_bottom_left.png"),
		imageCache.load("ui/skin/button_hover_bottom_middle.png"),
		imageCache.load("ui/skin/button_hover_bottom_right.png")
	},
	mSkinPressed{
		imageCache.load("ui/skin/button_pressed_top_left.png"),
		imageCache.load("ui/skin/button_pressed_top_middle.png"),
		imageCache.load("ui/skin/button_pressed_top_right.png"),
		imageCache.load("ui/skin/button_pressed_middle_left.png"),
		imageCache.load("ui/skin/button_pressed_middle_middle.png"),
		imageCache.load("ui/skin/button_pressed_middle_right.png"),
		imageCache.load("ui/skin/button_pressed_bottom_left.png"),
		imageCache.load("ui/skin/button_pressed_bottom_middle.png"),
		imageCache.load("ui/skin/button_pressed_bottom_right.png")
	}
{
	text(newText);

	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().connect(this, &Button::onMouseDown);
	eventHandler.mouseButtonUp().connect(this, &Button::onMouseUp);
	eventHandler.mouseMotion().connect(this, &Button::onMouseMove);

	mFont = &fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal);
}


Button::Button(std::string newText, ClickSignal::DelegateType clickHandler) : Button(newText)
{
	mSignal.connect(clickHandler);
}


Button::~Button()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect(this, &Button::onMouseDown);
	eventHandler.mouseButtonUp().disconnect(this, &Button::onMouseUp);
	eventHandler.mouseMotion().disconnect(this, &Button::onMouseMove);
}


void Button::type(Type type)
{
	mType = type;
}


void Button::toggle(bool toggle)
{
	mState = toggle ? State::Pressed : State::Normal;
}


bool Button::toggled() const
{
	return mState == State::Pressed;
}


void Button::fontSize(unsigned int pointSize)
{
	mFont = &fontCache.load(constants::FONT_PRIMARY, pointSize);
}


void Button::image(const std::string& path)
{
	mImage = &imageCache.load(path);
}


bool Button::hasImage() const
{
	return mImage != nullptr;
}


void Button::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!enabled() || !visible()) { return; }

	if (button == EventHandler::MouseButton::Left)
	{
		if (mRect.contains(Point<int>{x, y}))
		{
			if (mType == Type::BUTTON_NORMAL)
			{
				mState = State::Pressed;
			}
			else
			{
				mState = (mState == State::Pressed ? State::Normal : State::Pressed);
				mSignal();
			}
		}
	}
}


void Button::onMouseUp(EventHandler::MouseButton button, int x, int y)
{
	if (!enabled() || !visible()) { return; }

	if (button == EventHandler::MouseButton::Left)
	{
		if (mType == Type::BUTTON_NORMAL)
		{
			mState = State::Normal;

			if (mRect.contains(Point<int>{x, y}))
			{
				mSignal();
			}
		}
	}
}


void Button::onMouseMove(int x, int y, int /*dX*/, int /*dY*/)
{
	mMouseHover = mRect.contains(NAS2D::Point{x, y});
}


void Button::update()
{
	draw();
}


/**
 * Draws the button.
 */
void Button::draw()
{
	if (!visible()) { return; }

	auto& renderer = Utility<Renderer>::get();

	const auto& skin = (mState == State::Pressed) ? mSkinPressed :
		(enabled() && mMouseHover) ? mSkinHover : mSkinNormal;
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


