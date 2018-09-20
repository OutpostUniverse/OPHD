#pragma once

#include "NAS2D/NAS2D.h"

#include "Control.h"

#include <string>

class Button: public Control
{
public:
	enum Type
	{
		BUTTON_NORMAL,
		BUTTON_TOGGLE
	};

public:
	typedef NAS2D::Signals::Signal0<void> ClickCallback;

public:
	Button();
	virtual ~Button();

	void type(Type type);

	void toggle(bool toggle);
	bool toggled() const;

	void fontSize(size_t);

	void image(const std::string& path);
	bool hasImage() const;
	
	ClickCallback& click() { return mCallback; }

	virtual void update();

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMotion(int x, int y, int dX, int dY);

private:
	enum State
	{
		STATE_NORMAL,
		STATE_PRESSED
	};

private:
	virtual void draw();

private:
	State				mState = STATE_NORMAL;		/**< Current state of the Button. */
	Type				mType = BUTTON_NORMAL;		/**< Modifies Button behavior. */

	NAS2D::Image*		mImage = nullptr;			/**< Image to draw centered on the Button. */

	NAS2D::ImageList	mSkinNormal;
	NAS2D::ImageList	mSkinHover;
	NAS2D::ImageList	mSkinPressed;

	NAS2D::Font*		mFont = nullptr;			/**< Buttons can have different font sizes. */

	ClickCallback		mCallback;					/**< Object to notify when the Button is activated. */

	bool				mMouseHover = false;		/**< Mouse is within the bounds of the Button. */
};
