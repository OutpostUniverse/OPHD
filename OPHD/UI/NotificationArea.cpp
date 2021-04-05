#include "NotificationArea.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


static const std::map<NotificationArea::NotificationType, Rectangle<float>> NotificationIconRect
{
	{ NotificationArea::NotificationType::Critical, { 64, 64, 32, 32 } },
	{ NotificationArea::NotificationType::Information, { 32, 64, 32, 32 } },
	{ NotificationArea::NotificationType::Warning, { 96, 64, 32, 32 } }
};


static const std::map<NotificationArea::NotificationType, NAS2D::Color> NotificationIconColor
{
	{ NotificationArea::NotificationType::Critical, Color::Red },
	{ NotificationArea::NotificationType::Information, Color::Green },
	{ NotificationArea::NotificationType::Warning, Color::Yellow }
};



NotificationArea::NotificationArea() :
	mIcons{ imageCache.load("ui/icons.png") }
{
	auto& eventhandler = Utility<EventHandler>::get();

	eventhandler.mouseButtonDown().connect(this, &NotificationArea::onMouseDown);

	width(Width);
}


NotificationArea::~NotificationArea()
{
	auto& eventhandler = Utility<EventHandler>::get();

	eventhandler.mouseButtonDown().disconnect(this, &NotificationArea::onMouseDown);
}


void NotificationArea::push(const std::string& message, NotificationType type)
{
	mNotificationList.emplace_back(Notification{ message, type });
}


void NotificationArea::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (button != EventHandler::MouseButton::Left) { return; }

	NAS2D::Point clickPoint{ x, y };
}


void NotificationArea::update()
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawBox(rect());

	constexpr Rectangle<float> bgRect{128, 64, 32, 32};
	constexpr int offset = constants::MARGIN_TIGHT + 32;

	const int posX = positionX() + (Width / 2) - 16;

	int count = 1;
	for (auto& notification : mNotificationList)
	{
		auto position = Point<int>{ posX, positionY() + size().y - (offset * count) };
		renderer.drawSubImage(mIcons, position, bgRect, NotificationIconColor.at(notification.type));
		renderer.drawSubImage(mIcons, position, NotificationIconRect.at(notification.type), Color::Normal);

		renderer.drawBox(Rectangle<int>{ position.x, position.y, 32, 32 });

		count++;
	}
}
