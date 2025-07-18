#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include <libControls/TextArea.h>

#include "NotificationArea.h"

#include <NAS2D/Signal/Delegate.h>


struct MapCoordinate;


class NotificationWindow : public Window
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const MapCoordinate&)>;

public:
	NotificationWindow(TakeMeThereDelegate takeMeThereHandler);

	void notification(const NotificationArea::Notification&);

	void drawClientArea(NAS2D::Renderer& renderer) const override;

private:
	void onVisibilityChange(bool isVisible) override;
	void onOkayClicked();
	void onTakeMeThereClicked();

	const NAS2D::Image& mIcons;

	NotificationArea::Notification mNotification;
	Button btnOkay{"Okay", {this, &NotificationWindow::onOkayClicked}};
	Button btnTakeMeThere{"Take Me There", {this, &NotificationWindow::onTakeMeThereClicked}};
	TextArea mMessageArea;

	TakeMeThereDelegate mTakeMeThereHandler;
};
