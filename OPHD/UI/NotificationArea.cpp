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


static constexpr int Offset = constants::MARGIN_TIGHT + 32;


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

	const int posX = positionX() + (Width / 2) - 16;
	const int posY = positionY() + size().y - (Offset * static_cast<int>(mNotificationList.size()));

	mNotificationRectList.emplace_back(Rectangle<int>{ posX, posY, 32, 32 });
}


void NotificationArea::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (button != EventHandler::MouseButton::Left) { return; }

	const NAS2D::Point clickPoint{ x, y };

	size_t count = 0;
	for (auto& rect : mNotificationRectList)
	{
		if (rect.contains(clickPoint))
		{
			// fire off some event with notification here
			mNotificationList.erase(mNotificationList.begin() + count);
			mNotificationRectList.erase(mNotificationRectList.begin() + count);
			updateRectListPositions();
			return;
		}

		count++;
	}
}


void NotificationArea::positionChanged(int, int)
{
	Control::positionChanged(dX, dY);
	updateRectListPositions();
}


void NotificationArea::onResize()
{
	Control::onSizeChanged();
	updateRectListPositions();
}


void NotificationArea::updateRectListPositions()
{
	size_t count = 1;
	for (auto& rect : mNotificationRectList)
	{
		const int posX = positionX() + (Width / 2) - 16;
		const int posY = positionY() + size().y - (Offset * static_cast<int>(count));

		rect.startPoint({ posX, posY });

		count++;
	}
}


void NotificationArea::update()
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawBox(rect());

	constexpr Rectangle<float> bgRect{128, 64, 32, 32};

	size_t count = 0;
	for (auto& notification : mNotificationList)
	{
		auto& rect = mNotificationRectList.at(count);

		renderer.drawSubImage(mIcons, rect.startPoint(), bgRect, NotificationIconColor.at(notification.type));
		renderer.drawSubImage(mIcons, rect.startPoint(), NotificationIconRect.at(notification.type), Color::Normal);
		
		count++;
	}
}
