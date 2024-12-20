#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/TextArea.h>

#include "NotificationArea.h"

#include <NAS2D/Signal/Signal.h>


struct MapCoordinate;


class NotificationWindow : public Window
{
public:
	using TakeMeThereSignal = NAS2D::Signal<const MapCoordinate&>;

public:
	NotificationWindow();

	void notification(const NotificationArea::Notification&);

	TakeMeThereSignal::Source& takeMeThere() { return mTakeMeThereClicked; }

	void update() override;

private:
	void onOkayClicked();
	void onTakeMeThereClicked();

	const NAS2D::Image& mIcons;

	NotificationArea::Notification mNotification;
	Button btnOkay{"Okay", {this, &NotificationWindow::onOkayClicked}};
	Button btnTakeMeThere{"Take Me There", {this, &NotificationWindow::onTakeMeThereClicked}};
	TextArea mMessageArea;
	bool mTakeMeThereVisible{false};

	TakeMeThereSignal mTakeMeThereClicked;
};
