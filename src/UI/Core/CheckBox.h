#pragma once

#include "NAS2D/Signal.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Resources/Image.h"

#include "Control.h"

#include <string>

class CheckBox : public Control
{
public:
	typedef NAS2D::Signals::Signal<> ClickCallback;

public:
	CheckBox(std::string newText = "");
	~CheckBox() override;

	void checked(bool toggle);
	bool checked() const;

	ClickCallback& click();

	void update() final;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

	void onSizeChanged() final;
	void onTextChanged() final;
	
private:
	NAS2D::Image	mSkin;

	ClickCallback	mCallback;			/**< Object to notify when the Button is activated. */

	bool			mChecked = false;
};
