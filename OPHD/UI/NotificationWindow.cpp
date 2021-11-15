#include "NotificationWindow.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


NotificationWindow::NotificationWindow():
	mIcons{imageCache.load("ui/icons.png")}
{
	size({300, 220});

	add(btnOkay, {245, 195});
	btnOkay.size({50, 20});

	add(btnTakeMeThere, {10, 195});
	btnTakeMeThere.size({125, 20});
	btnTakeMeThere.hide();

	add(mMessageArea, {5, 65});
	mMessageArea.size({size().x - 10, 125});
	mMessageArea.font(constants::FONT_PRIMARY, constants::FontPrimaryNormal);
}


void NotificationWindow::notification(const NotificationArea::Notification& notification)
{
	mNotification = notification;
	title(mNotification.brief);
	mMessageArea.text(mNotification.message);
	mTakeMeThereVisible = mNotification.position.xy != Point<int>{-1, -1}; //\fixme magic value
}


void NotificationWindow::btnOkayClicked()
{
	hide();
}


void NotificationWindow::btnTakeMeThereClicked()
{
	mTakeMeThereClicked(mNotification.position);
	hide();
}


void NotificationWindow::update()
{
	if (!visible()) { return; }

	Window::update();

	btnTakeMeThere.visible(mTakeMeThereVisible); // bit of a hack

	const auto iconLocation = position() + Vector{10, 30};
	drawNotificationIcon(iconLocation, mNotification.type, mIcons);
}
