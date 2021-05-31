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


static const std::map<NotificationArea::NotificationType, std::string> NotificationText
{
	{ NotificationArea::NotificationType::Critical, "Critical" },
	{ NotificationArea::NotificationType::Information, "Information" },
	{ NotificationArea::NotificationType::Warning, "Warning" }
};


const Rectangle<float>& IconRectFromNotificationType(const NotificationArea::NotificationType type)
{
	return NotificationIconRect.at(type);
}


const Color ColorFromNotification(const NotificationArea::NotificationType type)
{
	return NotificationIconColor.at(type);
}


const std::string& StringFromNotificationType(const NotificationArea::NotificationType type)
{
	return NotificationText.at(type);
}


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


void NotificationArea::push(const std::string& brief, const std::string& message, NAS2D::Point<int> position, NotificationType type)
{
	mNotificationList.emplace_back(Notification{ brief, message, position, type });

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
			mNotificationClicked(mNotificationList.at(count));
			mNotificationList.erase(mNotificationList.begin() + count);
			mNotificationRectList.erase(mNotificationRectList.begin() + count);
			updateRectListPositions();
			return;
		}

		count++;
	}
}


void NotificationArea::onMove(NAS2D::Vector<int> displacement)
{
	Control::onMove(displacement);
	updateRectListPositions();
}


void NotificationArea::onResize()
{
	Control::onResize();
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

	size_t count = 0;
	for (auto& notification : mNotificationList)
	{
		auto& rect = mNotificationRectList.at(count);

		renderer.drawSubImage(mIcons, rect.startPoint(), { 128, 64, 32, 32 }, NotificationIconColor.at(notification.type));
		renderer.drawSubImage(mIcons, rect.startPoint(), NotificationIconRect.at(notification.type), Color::Normal);
		
		count++;
	}
}
