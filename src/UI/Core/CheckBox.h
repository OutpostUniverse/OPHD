#pragma once

#include "NAS2D/NAS2D.h"

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

	ClickCallback& click() { return mCallback; }

	virtual void update() final;

protected:
	void onMouseDown(EventHandler::MouseButton button, int x, int y);

	virtual void onFocusChanged() final;
	virtual void onSizeChanged() final;
	virtual void onTextChanged() final;
	virtual void visibilityChanged(bool visible) final;

	void _hook_events(bool hook);

private:
	NAS2D::Image	mSkin;				/**<  */

	ClickCallback	mCallback;			/**< Object to notify when the Button is activated. */

	bool			mChecked = false;	/**<  */
};
