#pragma once

#include "TextControl.h"

#include <NAS2D/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Resources/Image.h>

#include <string>


class CheckBox : public TextControl
{
public:
	using ClickCallback = NAS2D::Signals::Signal<>;

	CheckBox(std::string newText = "");
	~CheckBox() override;

	void checked(bool toggle);
	bool checked() const;

	ClickCallback& click();

	void update() override;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

	void onSizeChanged() override;
	void onTextChanged() override;

private:
	const NAS2D::Font* CBOX_FONT = nullptr;
	const NAS2D::Image& mSkin;

	ClickCallback mCallback; /**< Object to notify when the Button is activated. */

	bool mChecked = false;
};
