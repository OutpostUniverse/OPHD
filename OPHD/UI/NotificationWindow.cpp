#include "NotificationWindow.h"

#include "../Cache.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


NotificationWindow::NotificationWindow():
	mIcons{ imageCache.load("ui/icons.png") }
{
	size({ 350, 220 });

	add(btnOkay, { 295, 195 });
	btnOkay.size({ 50, 20 });
	btnOkay.click().connect(this, &NotificationWindow::btnOkayClicked);
}


void NotificationWindow::btnOkayClicked()
{
	hide();
}


void NotificationWindow::update()
{
	if (!visible()) { return; }

	Window::update();

	auto& renderer = Utility<Renderer>::get();

	Point<float> iconLocation = position() + Vector{ 10, 30 };

	renderer.drawSubImage(mIcons, iconLocation, { 128, 64, 32, 32 }, ColorFromNotification(mNotification.type));
	renderer.drawSubImage(mIcons, iconLocation, IconRectFromNotificationType(mNotification.type), Color::Normal);
}
