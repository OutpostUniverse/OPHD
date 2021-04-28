#pragma once

#include "TextControl.h"
#include "Label.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Resource/Image.h>

#include <string>


class UIContainer;


class RadioButton : public TextControl
{
public:
	using ClickSignal = NAS2D::Signal<>;

	RadioButton(std::string newText = "");
	~RadioButton() override;

	void checked(bool toggle);
	bool checked() const;

	void text(const std::string& text);
	const std::string& text() const;

	ClickSignal::Source& click();

	void update() override;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

	void onResize() override;
	void onTextChange() override;

	void parentContainer(UIContainer* parent);

private:
	const NAS2D::Font& mFont;
	const NAS2D::Image& mSkin;
	Label mLabel;
	ClickSignal mSignal; /**< Object to notify when the Button is activated. */
	UIContainer* mParentContainer{nullptr};
	bool mChecked{false};

	friend class UIContainer;
};
