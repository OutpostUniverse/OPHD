#pragma once

#include "Control.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Renderer/RectangleSkin.h>

#include <string>


class Button: public Control
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

	using ClickDelegate = NAS2D::Delegate<void()>;

	Button(std::string newText = {});
	Button(std::string newText, ClickDelegate clickHandler);
	Button(std::string text, NAS2D::Vector<int> size, ClickDelegate clickHandler);
	Button(const NAS2D::Image& image, ClickDelegate clickHandler);
	Button(const ButtonSkin& buttonSkin, ClickDelegate clickHandler);
	~Button() override;

	void type(Type type);

	void toggle(bool toggle);
	bool isPressed() const;

	void text(const std::string& text);
	const std::string& text() const;

	void font(const NAS2D::Font& font);

	void image(const std::string& path);
	bool hasImage() const;

protected:
	void draw() const override;

	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

private:
	const ButtonSkin mButtonSkin;
	const NAS2D::Image* mImage = nullptr;
	const NAS2D::Font* mFont = nullptr;
	std::string mText;
	ClickDelegate mClickHandler;

	Type mType = Type::Push;
	bool mIsPressed = false;
	bool mMouseHover = false;
};
