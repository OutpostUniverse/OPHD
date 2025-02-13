#pragma once

#include "TextControl.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Renderer/RectangleSkin.h>

#include <string>


class Button: public TextControl
{
public:
	enum class Type
	{
		Push,
		Toggle
	};

	struct ButtonSkin
	{
		NAS2D::RectangleSkin normal;
		NAS2D::RectangleSkin hover;
		NAS2D::RectangleSkin pressed;
	};

	using ClickSignal = NAS2D::Signal<>;

	Button(std::string newText = "");
	Button(std::string newText, ClickSignal::DelegateType clickHandler);
	Button(std::string text, NAS2D::Vector<int> size, ClickSignal::DelegateType clickHandler);
	Button(const ButtonSkin& buttonSkin, ClickSignal::DelegateType clickHandler);
	~Button() override;

	void type(Type type);

	void toggle(bool toggle);
	bool isPressed() const;

	void font(const NAS2D::Font& font);

	void image(const std::string& path);
	bool hasImage() const;

	ClickSignal::Source& click() { return mSignal; }

	void update() override;

protected:
	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	void draw() const override;

private:
	const ButtonSkin mButtonSkin;
	const NAS2D::Image* mImage = nullptr; /**< Image to draw centered on the Button. */
	const NAS2D::Font* mFont = nullptr; /**< Buttons can have different font sizes. */

	ClickSignal mSignal; /**< Object to notify when the Button is activated. */

	Type mType = Type::Push; /**< Modifies Button behavior. */
	bool mIsPressed = false; /**< Current state of the Button. */
	bool mMouseHover = false; /**< Mouse is within the bounds of the Button. */
};
