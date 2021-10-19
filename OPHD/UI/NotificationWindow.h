#pragma once

#include "Core/Button.h"
#include "Core/TextArea.h"
#include "Core/Window.h"

#include "NotificationArea.h"

#include <NAS2D/Signal/Signal.h>


struct MapCoordinate;


class NotificationWindow : public Window
{
public:
	using Signal = NAS2D::Signal<const MapCoordinate&>;

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
	Button btnOkay{"Okay", {this, &NotificationWindow::btnOkayClicked}};
	Button btnTakeMeThere{"Take Me There", {this, &NotificationWindow::btnTakeMeThereClicked}};
	TextArea mMessageArea;
	bool mTakeMeThereVisible{false};

	Signal mTakeMeThereClicked;
};
