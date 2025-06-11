#pragma once

#include "TextControl.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>

#include <string>


namespace NAS2D
{
	class Font;
	class Image;
}


class CheckBox : public TextControl
{
public:
	using ClickDelegate = NAS2D::Delegate<void()>;

	CheckBox(std::string newText = "", ClickDelegate clickHandler = {});
	~CheckBox() override;

	void checked(bool toggle);
	bool checked() const;

	void update() override;
	void draw() const override;

protected:
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);

	void onResize() override;
	void onTextChange() override;

private:
	const NAS2D::Font& mFont;
	const NAS2D::Image& mSkin;

	ClickDelegate mClickHandler;

	bool mChecked = false;
};
