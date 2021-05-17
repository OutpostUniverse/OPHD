#pragma once

#include "Core/Window.h"
#include "Core/Button.h"

#include "NotificationArea.h"

class NotificationWindow : public Window
{
public:
	NotificationWindow();
	~NotificationWindow() = default;

	void notification(const NotificationArea::Notification& notification)
	{
		mNotification = notification;
	}

	void update() override;

private:
	void btnOkayClicked();

	const NAS2D::Image& mIcons;

	NotificationArea::Notification mNotification;
	Button btnOkay{ "Okay" };
};
