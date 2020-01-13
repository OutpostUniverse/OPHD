#pragma once

#include "NAS2D/Signal.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Resources/Image.h"

#include "Control.h"

#include <string>

class CheckBox : public Control
{
public:
	typedef NAS2D::Signals::Signal0<void> ClickCallback;

public:
	CheckBox();
	virtual ~CheckBox();

	void checked(bool toggle);
	bool checked() const;

	ClickCallback& click();

	virtual void update() final;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

	virtual void onSizeChanged() final;
	virtual void onTextChanged() final;
	
private:
	NAS2D::Image	mSkin;				/**<  */

	ClickCallback	mCallback;			/**< Object to notify when the Button is activated. */

	bool			mChecked = false;	/**<  */
};
