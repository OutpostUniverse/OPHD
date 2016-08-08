#include "Button.h"

using namespace std;

Button::Button():	mState(STATE_NORMAL),
					mType(BUTTON_NORMAL),
					mUsesImage(false),
					mMouseHover(false)
{
	Utility<EventHandler>::get().mouseButtonDown().Connect(this, &Button::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().Connect(this, &Button::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().Connect(this, &Button::onMouseMotion);
	hasFocus(true);
}


Button::~Button()
{
	Utility<EventHandler>::get().mouseButtonDown().Disconnect(this, &Button::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().Disconnect(this, &Button::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().Disconnect(this, &Button::onMouseMotion);
}


void Button::type(Type type)
{
	type ? mType = BUTTON_TOGGLE : mType = BUTTON_NORMAL;
}


void Button::toggle(bool toggle)
{
	toggle ? mState = STATE_PRESSED : mState = STATE_NORMAL;
}


bool Button::toggled() const
{
	if(mState == STATE_PRESSED)
		return true;

	return false;
}


void Button::image(const std::string& path)
{
	mImage = Image(path);
	mUsesImage = true;
}


bool Button::hasImage() const
{
	return mImage.loaded();
}


void Button::onMouseDown(MouseButton button, int x, int y)
{
	if(!enabled() || !visible() || !hasFocus())
		return;

	if(button == BUTTON_LEFT)
	{
		Point_2d click(x, y);


		if(isPointInRect(click, rect()))
		{
			if(mType == BUTTON_NORMAL)
			{
				mState = STATE_PRESSED;
			}
			else
			{
				mState == STATE_PRESSED ? mState = STATE_NORMAL : mState = STATE_PRESSED;
				mCallback();
			}
		}
	}
}


void Button::onMouseUp(MouseButton button, int x, int y)
{
	if(!enabled() || !visible() || !hasFocus())
		return;

	if(button == BUTTON_LEFT)
	{
		Point_2d click(x, y);
		
		if(mType == BUTTON_NORMAL)
		{
			mState = STATE_NORMAL;

			if(isPointInRect(click, rect()))
				mCallback();
		}
	}
}


void Button::onMouseMotion(int x, int y, int dX, int dY)
{
	if (isPointInRect(x, y, rect().x(), rect().y(), rect().w(), rect().h()))
	{
		mMouseHover = true;
		return;
	}

	mMouseHover = false;
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
	if(!visible())
		return;

	Renderer& r = Utility<Renderer>::get();

	if (mState == STATE_NORMAL)
	{
		r.drawBoxFilled(rect(), 225, 225, 225);
		r.drawBox(rect(), 175, 175, 175);
	}
	else //(mState == STATE_PRESSED)
	{
		if (mType == BUTTON_NORMAL)
			r.drawBoxFilled(rect(), 200, 215, 245);
		else //(mType == BUTTON_TOGGLE)
			r.drawBoxFilled(rect(), 170, 210, 245);

		r.drawBox(rect(), 0, 85, 155);
	}

	if (enabled() && mMouseHover && mState != STATE_PRESSED)
		r.drawBox(rect(), 0, 120, 215);

	if (mUsesImage)
		r.drawImage(mImage, rect().x() + (rect().w() / 2) - (mImage.width() / 2), rect().y() + (rect().h() / 2) - (mImage.height() / 2));
	else
		if (fontSet() && !text().empty())
			r.drawText(font(), text(), static_cast<int>(rect().x() + (rect().w() / 2) - (font().width(text()) / 2)), static_cast<int>(rect().y() + (rect().h() / 2) - (font().height() / 2)), 0, 0, 0);

	if (!enabled())
		r.drawBoxFilled(rect(), 200, 200, 200, 100);
}


