// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Button.h"

#include "../../Common.h"
#include "../../Constants.h"
#include "../../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"
#include "NAS2D/MathUtils.h"

using namespace NAS2D;


Button::Button(std::string newText)
{
	text(newText);

	Utility<EventHandler>::get().mouseButtonDown().connect(this, &Button::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().connect(this, &Button::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().connect(this, &Button::onMouseMove);
	hasFocus(true);

	mSkinNormal.push_back(Image("ui/skin/button_top_left.png"));
	mSkinNormal.push_back(Image("ui/skin/button_top_middle.png"));
	mSkinNormal.push_back(Image("ui/skin/button_top_right.png"));
	mSkinNormal.push_back(Image("ui/skin/button_middle_left.png"));
	mSkinNormal.push_back(Image("ui/skin/button_middle_middle.png"));
	mSkinNormal.push_back(Image("ui/skin/button_middle_right.png"));
	mSkinNormal.push_back(Image("ui/skin/button_bottom_left.png"));
	mSkinNormal.push_back(Image("ui/skin/button_bottom_middle.png"));
	mSkinNormal.push_back(Image("ui/skin/button_bottom_right.png"));

	mSkinHover.push_back(Image("ui/skin/button_hover_top_left.png"));
	mSkinHover.push_back(Image("ui/skin/button_hover_top_middle.png"));
	mSkinHover.push_back(Image("ui/skin/button_hover_top_right.png"));
	mSkinHover.push_back(Image("ui/skin/button_hover_middle_left.png"));
	mSkinHover.push_back(Image("ui/skin/button_hover_middle_middle.png"));
	mSkinHover.push_back(Image("ui/skin/button_hover_middle_right.png"));
	mSkinHover.push_back(Image("ui/skin/button_hover_bottom_left.png"));
	mSkinHover.push_back(Image("ui/skin/button_hover_bottom_middle.png"));
	mSkinHover.push_back(Image("ui/skin/button_hover_bottom_right.png"));

	mSkinPressed.push_back(Image("ui/skin/button_pressed_top_left.png"));
	mSkinPressed.push_back(Image("ui/skin/button_pressed_top_middle.png"));
	mSkinPressed.push_back(Image("ui/skin/button_pressed_top_right.png"));
	mSkinPressed.push_back(Image("ui/skin/button_pressed_middle_left.png"));
	mSkinPressed.push_back(Image("ui/skin/button_pressed_middle_middle.png"));
	mSkinPressed.push_back(Image("ui/skin/button_pressed_middle_right.png"));
	mSkinPressed.push_back(Image("ui/skin/button_pressed_bottom_left.png"));
	mSkinPressed.push_back(Image("ui/skin/button_pressed_bottom_middle.png"));
	mSkinPressed.push_back(Image("ui/skin/button_pressed_bottom_right.png"));

	mFont = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
}


Button::~Button()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &Button::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().disconnect(this, &Button::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &Button::onMouseMove);

	delete mImage;
}


void Button::type(Type type)
{
	mType = type;
}


void Button::toggle(bool toggle)
{
	mState = toggle ? STATE_PRESSED : STATE_NORMAL;
}


bool Button::toggled() const
{
	return mState == STATE_PRESSED;
}


void Button::fontSize(size_t size)
{
	mFont = Utility<FontManager>::get().font(constants::FONT_PRIMARY, size);
}


void Button::image(const std::string& path)
{
	mImage = new Image(path);
}


bool Button::hasImage() const
{
	return mImage->loaded();
}


void Button::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!enabled() || !visible() || !hasFocus()) { return; }

	if(button == EventHandler::MouseButton::BUTTON_LEFT)
	{
		Point<int> click(x, y);

		if(rect().contains(click))
		{
			if(mType == Type::BUTTON_NORMAL)
			{
				mState = STATE_PRESSED;
			}
			else
			{
				mState = (mState == STATE_PRESSED ? STATE_NORMAL : STATE_PRESSED);
				mCallback();
			}
		}
	}
}


void Button::onMouseUp(EventHandler::MouseButton button, int x, int y)
{
	if(!enabled() || !visible() || !hasFocus()) { return; }

	if(button == EventHandler::MouseButton::BUTTON_LEFT)
	{
		Point<int> click(x, y);
		
		if(mType == Type::BUTTON_NORMAL)
		{
			mState = STATE_NORMAL;

			if (rect().contains(click))
			{
				mCallback();
			}
		}
	}
}


void Button::onMouseMove(int x, int y, int /*dX*/, int /*dY*/)
{
	mMouseHover = mRect.to<int>().contains(NAS2D::Point{x, y});
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

	Renderer& r = Utility<Renderer>::get();

	if (enabled() && mMouseHover && mState != STATE_PRESSED)
	{
		r.drawImageRect(rect(), mSkinHover);
	}
	else if (mState == STATE_NORMAL)
	{
		r.drawImageRect(rect(), mSkinNormal);
	}
	else
	{
		r.drawImageRect(rect(), mSkinPressed);
	}

	if (mImage)
	{
		r.drawImage(*mImage, rect().center() - mImage->size() / 2);
	}
	else
	{
		// force text to be drawn on integer bounds, otherwise it can look 'fuzzy' due to texture blending
		const auto textPosition = rect().center().to<int>() - NAS2D::Vector{mFont->width(text()), mFont->height()} / 2;
		r.drawText(*mFont, text(), textPosition, NAS2D::Color::White);
	}

	/// \fixme	Naive... would rather set a b&w shader instead.
	if (!enabled())
	{
		r.drawBoxFilled(rect(), NAS2D::Color{125, 125, 125, 100});
	}
}


