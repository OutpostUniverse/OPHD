#include "NotificationArea.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <utility>


using namespace NAS2D;


namespace
{
	constexpr auto IconSize = NAS2D::Vector{32, 32};
	constexpr auto IconPadding = NAS2D::Vector{8, constants::MarginTight / 2};
	constexpr auto IconPaddedSize = IconSize + IconPadding * 2;
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

	width(IconPaddedSize.x);
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


NAS2D::Rectangle<int> NotificationArea::notificationRect(std::size_t index)
{
	auto rectPosition = position() + NAS2D::Vector{IconPadding.x, size().y - IconPaddedSize.y * static_cast<int>(index + 1)};
	return NAS2D::Rectangle<int>::Create(rectPosition, IconSize);
}


std::size_t NotificationArea::notificationIndex(NAS2D::Point<int> pixelPosition)
{
	const auto estimatedIndex = static_cast<std::size_t>((mRect.endPoint().y - pixelPosition.y) / IconPaddedSize.y);
	// Icon is clickable, but padding area around icon is not clickable
	if (notificationRect(estimatedIndex).contains(pixelPosition))
	{
		return estimatedIndex;
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

	const auto index = notificationIndex({x, y});
	if (index != NoSelection)
	{
		if (button == EventHandler::MouseButton::Left)
		{
			mNotificationClicked(mNotificationList.at(index));
		}

		mNotificationList.erase(mNotificationList.begin() + index);
		onMouseMove(x, y, 0, 0);
	}
}


void NotificationArea::onMouseMove(int x, int y, int /*dX*/, int /*dY*/)
{
	if (!rect().contains({x, y})) { return; }
	mNotificationIndex = notificationIndex({x, y});
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
			const auto textPadding = Vector<int>{4, 2};
			const auto textAreaSize = mFont.size(notification.brief) + textPadding * 2;
			const auto briefPosition = rect.startPoint() + NAS2D::Vector{-IconPadding.x - textAreaSize.x, (rect.height - textAreaSize.y) / 2};
			const auto notificationBriefRect = NAS2D::Rectangle<int>::Create(briefPosition, textAreaSize);
			const auto textPosition = briefPosition + textPadding;

			renderer.drawBoxFilled(notificationBriefRect, Color::DarkGray);
			renderer.drawBox(notificationBriefRect, Color::Black);
			renderer.drawText(mFont, notification.brief, textPosition, Color::White);
		}

		count++;
	}
}
