#pragma once

#include "Core/Button.h"
#include "Core/TextArea.h"
#include "Core/Window.h"

#include "NotificationArea.h"

#include <NAS2D/Signal/Signal.h>

class NotificationWindow : public Window
{
public:
	using Signal = NAS2D::Signal<NAS2D::Point<int>>;

public:
	NotificationWindow();

	void notification(const NotificationArea::Notification&);

	Signal& takeMeThere() { return mTakeMeThereClicked; }

	void update() override;

private:
	void btnOkayClicked();
	void btnTakeMeThereClicked();

	const NAS2D::Image& mIcons;

	NotificationArea::Notification mNotification;
	Button btnOkay{ "Okay" };
	Button btnTakeMeThere{ "Take Me There" };
	TextArea mMessageArea;
	bool mTakeMeThereVisible{ false };

	Signal mTakeMeThereClicked;
};