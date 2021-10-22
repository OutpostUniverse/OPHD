#pragma once

#include "TextControl.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>

#include <string>


namespace NAS2D
{
	class Font;
	class Image;
}


class CheckBox : public TextControl
{
public:
	using ClickSignal = NAS2D::Signal<>;

	CheckBox(std::string newText = "");
	CheckBox(std::string newText, ClickSignal::DelegateType clickHandler);
	~CheckBox() override;

	void checked(bool toggle);
	bool checked() const;

	ClickSignal::Source& click();

	void update() override;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

	void onResize() override;
	void onTextChange() override;

private:
	const NAS2D::Font& mFont;
	const NAS2D::Image& mSkin;

	ClickSignal mSignal; /**< Object to notify when the Button is activated. */

	bool mChecked = false;
};
