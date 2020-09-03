#pragma once

#include "TextControl.h"

#include <NAS2D/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Resources/Image.h>
#include <NAS2D/Resources/Font.h>
#include <NAS2D/Renderer/RectangleSkin.h>

#include <string>


class Button: public TextControl
{
public:
	enum class Type
	{
		BUTTON_NORMAL,
		BUTTON_TOGGLE
	};

public:
	using ClickCallback = NAS2D::Signals::Signal<>;

public:
	Button(std::string newText = "");
	~Button() override;

	void type(Type type);

	void toggle(bool toggle);
	bool toggled() const;

	void fontSize(unsigned int pointSize);

	void image(const std::string& path);
	bool hasImage() const;
	
	ClickCallback& click() { return mCallback; }

	void update() override;

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int dX, int dY);

private:
	enum class State
	{
		Normal,
		Pressed
	};

private:
	void draw() override;

private:
	State mState = State::Normal; /**< Current state of the Button. */
	Type mType = Type::BUTTON_NORMAL; /**< Modifies Button behavior. */

	const NAS2D::Image* mImage = nullptr; /**< Image to draw centered on the Button. */

	NAS2D::RectangleSkin mSkinNormal;
	NAS2D::RectangleSkin mSkinHover;
	NAS2D::RectangleSkin mSkinPressed;

	const NAS2D::Font* mFont = nullptr; /**< Buttons can have different font sizes. */

	ClickCallback mCallback; /**< Object to notify when the Button is activated. */

	bool mMouseHover = false; /**< Mouse is within the bounds of the Button. */
};
