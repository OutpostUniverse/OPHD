#pragma once

#include "NAS2D/Signal.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Resources/Image.h"

#include "Control.h"
#include "Label.h"

#include <string>

class UIContainer;

class RadioButton : public Control
{
public:
	typedef NAS2D::Signals::Signal<> ClickCallback;

public:
	RadioButton(std::string newText = "");
	~RadioButton() override;

	void checked(bool toggle);
	bool checked() const;

	void text(const std::string& text);
	const std::string& text() const;

	ClickCallback& click();

	void update() final;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

	void onSizeChanged() final;
	void onTextChanged() final;

	void parentContainer(UIContainer* parent);

private:
	NAS2D::Image	mSkin;
	Label			mLabel;
	ClickCallback	mCallback;			/**< Object to notify when the Button is activated. */
	UIContainer*	mParentContainer{nullptr};
	bool			mChecked{false};

	friend class UIContainer;
};
