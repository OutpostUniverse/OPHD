#include "NotificationArea.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <utility>


using namespace NAS2D;


namespace
{
	constexpr int Width = 48;
	constexpr int Offset = constants::MarginTight + 32;
	constexpr std::size_t NoSelection = SIZE_MAX;


	const std::map<NotificationArea::NotificationType, Rectangle<float>> NotificationIconRect
	{
		{NotificationArea::NotificationType::Critical, {64, 64, 32, 32}},
		{NotificationArea::NotificationType::Information, {32, 64, 32, 32}},
		{NotificationArea::NotificationType::Warning, {96, 64, 32, 32}}
	};


	const std::map<NotificationArea::NotificationType, NAS2D::Color> NotificationIconColor
	{
		{NotificationArea::NotificationType::Critical, Color::Red},
		{NotificationArea::NotificationType::Information, Color::Green},
		{NotificationArea::NotificationType::Warning, Color::Yellow}
	};


	const std::map<NotificationArea::NotificationType, std::string> NotificationText
	{
		{NotificationArea::NotificationType::Critical, "Critical"},
		{NotificationArea::NotificationType::Information, "Information"},
		{NotificationArea::NotificationType::Warning, "Warning"}
	};
}


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


NotificationArea::NotificationArea() :
	mIcons{imageCache.load("ui/icons.png")},
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	mNotificationIndex{NoSelection}
{
	auto& eventhandler = Utility<EventHandler>::get();

	eventhandler.mouseButtonDown().connect(this, &NotificationArea::onMouseDown);
	eventhandler.mouseMotion().connect(this, &NotificationArea::onMouseMove);

	width(Width);
}


NotificationArea::~NotificationArea()
{
	auto& eventhandler = Utility<EventHandler>::get();

	eventhandler.mouseButtonDown().disconnect(this, &NotificationArea::onMouseDown);
	eventhandler.mouseMotion().disconnect(this, &NotificationArea::onMouseMove);
}


void NotificationArea::push(Notification notification)
{
	mNotificationList.push_back(std::move(notification));
}


void NotificationArea::clear()
{
	mNotificationList.clear();
}


NAS2D::Rectangle<int> NotificationArea::notificationRect(std::size_t count)
{
	auto rectPosition = position() + NAS2D::Vector{(Width / 2) - 16, size().y - Offset * static_cast<int>(count + 1)};
	return NAS2D::Rectangle<int>::Create(rectPosition, NAS2D::Vector{32, 32});
}


std::size_t NotificationArea::notificationIndex(NAS2D::Point<int> pixelPosition)
{
	for (size_t count = 0; count < mNotificationList.size(); ++count)
	{
		const auto& rect = notificationRect(count);
		if (rect.contains(pixelPosition))
		{
			return count;
		}
	}
	return NoSelection;
}


void NotificationArea::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (button != EventHandler::MouseButton::Left &&
		button != EventHandler::MouseButton::Right)
	{
		return;
	}

	const auto count = notificationIndex({x, y});
	if (count != NoSelection)
	{
		if (button == EventHandler::MouseButton::Left)
		{
			mNotificationClicked(mNotificationList.at(count));
		}

		mNotificationList.erase(mNotificationList.begin() + count);
		onMouseMove(x, y, 0, 0);
	}
}


void NotificationArea::onMouseMove(int x, int y, int /*dX*/, int /*dY*/)
{
	if (!rect().contains({x, y})) { return; }

	mNotificationIndex = notificationIndex({x, y});
	if (mNotificationIndex != NoSelection)
	{
		const auto& rect = notificationRect(mNotificationIndex);

		const int stringWidth = mFont.width(mNotificationList[mNotificationIndex].brief) + 8;
		const int briefPositionX = positionX() - stringWidth;
		const int briefPositionY = rect.y + (rect.height / 2) - (mFont.height() / 2) - 2;

		mNotificationBriefRect = {briefPositionX, briefPositionY, stringWidth, mFont.height() + 4};
	}
}


void NotificationArea::update()
{
	auto& renderer = Utility<Renderer>::get();

	size_t count = 0;
	for (auto& notification : mNotificationList)
	{
		const auto& rect = notificationRect(count);

		renderer.drawSubImage(mIcons, rect.startPoint(), {128, 64, 32, 32}, NotificationIconColor.at(notification.type));
		renderer.drawSubImage(mIcons, rect.startPoint(), NotificationIconRect.at(notification.type), Color::Normal);

		if (mNotificationIndex == count)
		{
			renderer.drawBoxFilled(mNotificationBriefRect, Color::DarkGray);
			renderer.drawBox(mNotificationBriefRect, Color::Black);

			const auto textPosition = mNotificationBriefRect.startPoint() + Vector<int>{4, 2};
			renderer.drawText(mFont, notification.brief, textPosition, Color::White);
		}

		count++;
	}
}
