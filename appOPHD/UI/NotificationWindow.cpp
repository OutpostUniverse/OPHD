#include "NotificationWindow.h"

#include "../Cache.h"


NotificationWindow::NotificationWindow(TakeMeThereDelegate takeMeThereHandler):
	mIcons{imageCache.load("ui/icons.png")},
	mTakeMeThereHandler{takeMeThereHandler}
{
	size({300, 220});

	add(btnOkay, {245, 195});
	btnOkay.size({50, 20});

	add(btnTakeMeThere, {10, 195});
	btnTakeMeThere.size({125, 20});
	btnTakeMeThere.hide();

	add(mMessageArea, {5, 65});
	mMessageArea.size({size().x - 10, 125});
}


void NotificationWindow::notification(const NotificationArea::Notification& notification)
{
	mNotification = notification;
	title(mNotification.brief);
	mMessageArea.text(mNotification.message);
}


void NotificationWindow::onVisibilityChange(bool isVisible)
{
	Window::onVisibilityChange(isVisible);

	if (!enabled() || !visible()) { return; }
	btnTakeMeThere.visible(mNotification.hasMapCoordinate());
}


void NotificationWindow::onOkayClicked()
{
	hide();
}


void NotificationWindow::onTakeMeThereClicked()
{
	if (mTakeMeThereHandler) { mTakeMeThereHandler(mNotification.position); }
	hide();
}


void NotificationWindow::drawClientArea(NAS2D::Renderer& /*renderer*/) const
{
	const auto iconLocation = position() + NAS2D::Vector{10, 30};
	drawNotificationIcon(iconLocation, mNotification.type, mIcons);
}
