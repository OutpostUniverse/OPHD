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


Button::Button()
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &Button::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().connect(this, &Button::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().connect(this, &Button::onMouseMotion);
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
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &Button::onMouseMotion);

	delete mImage;
}


void Button::type(Type type)
{
	mType = type ? BUTTON_TOGGLE : BUTTON_NORMAL;
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
		Point_2d click(x, y);

		if(rect().contains(click))
		{
			if(mType == BUTTON_NORMAL)
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
		Point_2d click(x, y);
		
		if(mType == BUTTON_NORMAL)
		{
			mState = STATE_NORMAL;

			if (rect().contains(click))
			{
				mCallback();
			}
		}
	}
}


void Button::onMouseMotion(int x, int y, int /*dX*/, int /*dY*/)
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
		r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkinHover);
	}
	else if (mState == STATE_NORMAL)
	{
		r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkinNormal);
	}
	else
	{
		r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkinPressed);
	}

	if (mImage)
	{
		r.drawImage(*mImage, rect().x() + (rect().width() / 2) - (mImage->width() / 2), rect().y() + (rect().height() / 2) - (mImage->height() / 2));
	}
	else
	{
		// force text to be drawn on integer bounds, otherwise it can look 'fuzzy' due to texture blending
		int posX = static_cast<int>(rect().x() + (rect().width() / 2) - (mFont->width(text()) / 2));
		int posY = static_cast<int>(rect().y() + (rect().height() / 2) - (mFont->height() / 2));

		r.drawText(*mFont, text(), static_cast<float>(posX), static_cast<float>(posY), 255, 255, 255);
	}

	/// \fixme	Naive... would rather set a b&w shader instead.
	if (!enabled())
	{
		r.drawBoxFilled(rect(), 125, 125, 125, 100);
	}
}


