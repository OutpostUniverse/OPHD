#pragma once

#include "Core/Button.h"
#include "Core/TextArea.h"
#include "Core/Window.h"

#include "NotificationArea.h"

class NotificationWindow : public Window
{
public:
	NotificationWindow();

	void notification(const NotificationArea::Notification&);

	void update() override;

private:
	void btnOkayClicked();

	const NAS2D::Image& mIcons;

	NotificationArea::Notification mNotification;
	Button btnOkay{ "Okay" };
	TextArea mMessageArea;
};
