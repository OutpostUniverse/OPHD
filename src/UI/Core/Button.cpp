#include "Button.h"

using namespace std;

Button::Button():	mState(STATE_NORMAL),
					mType(BUTTON_NORMAL),
					mUsesImage(false)
{
	Utility<EventHandler>::get().mouseButtonDown().Connect(this, &Button::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().Connect(this, &Button::onMouseUp);
}


Button::~Button()
{
	Utility<EventHandler>::get().mouseButtonDown().Disconnect(this, &Button::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().Disconnect(this, &Button::onMouseUp);
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
	if(!enabled() || !visible())
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
	if(!enabled() || !visible())
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

	r.drawBoxFilled(rect(), COLOR_SILVER.red(), COLOR_SILVER.green(), COLOR_SILVER.blue());
	r.drawBox(rect(), 0, 0, 0);

	// Determine a text position so it's centered within the button.
	Point_2d pos;
	if(fontSet())
		pos = Point_2d((rect().x() + (rect().w() / 2)) - (font().width(text()) / 2), (rect().y() + (rect().h() / 2)) - (font().height() / 2));

	if(!enabled())
	{
		drawUp();
	
		if(mUsesImage)
			r.drawImage(mImage, rect().x() + (rect().w() / 2) - (mImage.width() / 2),  rect().y() + (rect().h() / 2) - (mImage.height() / 2));
		else
		{
			if(fontSet())
				r.drawText(font(), text(), pos.x(), pos.y(), COLOR_GREY.red(), COLOR_GREY.green(), COLOR_GREY.blue());
		}

		r.drawBoxFilled(rect(), 200, 200, 200, 100);
	}
	else if(mState == STATE_NORMAL)
	{
		drawUp();

		if(mUsesImage)
			r.drawImage(mImage, rect().x() + (rect().w() / 2) - (mImage.width() / 2),  rect().y() + (rect().h() / 2) - (mImage.height() / 2));
		else
		{
			if(fontSet())
				r.drawText(font(), text(), pos.x(), pos.y(), 0, 0, 0);
		}
	}
	else //(mState == STATE_PRESSED)
	{
		drawDown();

		if(mUsesImage)
			r.drawImage(mImage, rect().x() + (rect().w() / 2) - (mImage.width() / 2) + 1,  rect().y() + (rect().h() / 2) - (mImage.height() / 2) + 1);
		else
		{
			if(fontSet())
				r.drawText(font(), text(), pos.x() + 1, pos.y() + 1, 0, 0, 0);
		}
	}
}


void Button::drawUp()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawLine(rect().x() + 1, rect().y() + 1, rect().x() + rect().w() - 1, rect().y() + 1, COLOR_WHITE);
	r.drawLine(rect().x() + 1, rect().y() + 1, rect().x() + 1, rect().y() + rect().h() - 0.5, COLOR_WHITE);

	r.drawLine(rect().x() + 2, rect().y() + rect().h() - 1, rect().x() + rect().w() - 2, rect().y() + rect().h() - 1, COLOR_GREY);
	r.drawLine(rect().x() + rect().w() - 1, rect().y() + 2, rect().x() + rect().w() - 1, rect().y() + rect().h() - 0.5, COLOR_GREY);
}


void Button::drawDown()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawLine(rect().x() + 1, rect().y() + 1, rect().x() + rect().w() - 1, rect().y() + 1, COLOR_GREY);
	r.drawLine(rect().x() + 1, rect().y() + 1, rect().x() + 1, rect().y() + rect().h() - 1, COLOR_GREY);

	r.drawLine(rect().x() + 2, rect().y() + rect().h() - 1, rect().x() + rect().w() - 2, rect().y() + rect().h() - 1, COLOR_WHITE);
	r.drawLine(rect().x() + rect().w() - 1, rect().y() + 2, rect().x() + rect().w() - 1, rect().y() + rect().h() - 1, COLOR_WHITE);
}

