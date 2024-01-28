#include "NotificationArea.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/Image.h>

#include <utility>


using namespace NAS2D;


namespace
{
	constexpr auto IconSize = NAS2D::Vector{32, 32};
	constexpr auto IconPadding = NAS2D::Vector{8, constants::MarginTight / 2};
	constexpr auto IconPaddedSize = IconSize + IconPadding * 2;
	constexpr std::size_t NoSelection = SIZE_MAX;


	struct IconDrawParameters
	{
		NAS2D::Rectangle<int> iconRect;
		NAS2D::Color color;
	};


	const std::map<NotificationArea::NotificationType, IconDrawParameters> NotificationIconDrawParameters
	{
		{NotificationArea::NotificationType::Critical, {{{64, 64}, {32, 32}}, Color::Red}},
		{NotificationArea::NotificationType::Information, {{{128, 64}, {32, 32}}, Color::Blue}},
		{NotificationArea::NotificationType::Success, {{{32, 64}, {32, 32}}, Color::Green}},
		{NotificationArea::NotificationType::Warning, {{{96, 64}, {32, 32}}, {255, 165, 0}}}
	};
}


void drawNotificationIcon(NAS2D::Point<int> position, NotificationArea::NotificationType type, const NAS2D::Image& icons)
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto& iconDrawParameters = NotificationIconDrawParameters.at(type);
	renderer.drawSubImage(icons, position, {{160, 64}, {32, 32}}, iconDrawParameters.color);
	renderer.drawSubImage(icons, position, iconDrawParameters.iconRect, Color::Normal);
}


NotificationArea::NotificationArea() :
	mIcons{imageCache.load("ui/icons.png")},
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	mNotificationIndex{NoSelection}
{
	auto& eventhandler = Utility<EventHandler>::get();

	eventhandler.mouseButtonDown().connect({this, &NotificationArea::onMouseDown});
	eventhandler.mouseMotion().connect({this, &NotificationArea::onMouseMove});

	width(IconPaddedSize.x);
}


NotificationArea::~NotificationArea()
{
	auto& eventhandler = Utility<EventHandler>::get();

	eventhandler.mouseButtonDown().disconnect({this, &NotificationArea::onMouseDown});
	eventhandler.mouseMotion().disconnect({this, &NotificationArea::onMouseMove});
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
	return NAS2D::Rectangle{rectPosition, IconSize};
}


std::size_t NotificationArea::notificationIndex(NAS2D::Point<int> pixelPosition)
{
	const auto index = static_cast<std::size_t>((mRect.endPoint().y - pixelPosition.y) / IconPaddedSize.y);
	// Icon is clickable, but padding area around icon is not clickable
	if (index < mNotificationList.size() && notificationRect(index).contains(pixelPosition))
	{
		return index;
	}
	return NoSelection;
}


void NotificationArea::onMouseDown(EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (button != EventHandler::MouseButton::Left &&
		button != EventHandler::MouseButton::Right)
	{
		return;
	}

	const auto index = notificationIndex(position);
	if (index != NoSelection)
	{
		if (button == EventHandler::MouseButton::Left)
		{
			mNotificationClicked(mNotificationList.at(index));
		}

		mNotificationList.erase(mNotificationList.begin() + static_cast<std::ptrdiff_t>(index));
		onMouseMove(position, {0, 0});
	}
}


void NotificationArea::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	if (!rect().contains(position)) { return; }
	mNotificationIndex = notificationIndex(position);
}


void NotificationArea::update()
{
	auto& renderer = Utility<Renderer>::get();

	size_t count = 0;
	for (auto& notification : mNotificationList)
	{
		const auto& rect = notificationRect(count);
		drawNotificationIcon(rect.position, notification.type, mIcons);

		if (mNotificationIndex == count)
		{
			const auto textPadding = Vector<int>{4, 2};
			const auto textAreaSize = mFont.size(notification.brief) + textPadding * 2;
			const auto briefPosition = rect.position + NAS2D::Vector{-IconPadding.x - textAreaSize.x, (rect.size.y - textAreaSize.y) / 2};
			const auto notificationBriefRect = NAS2D::Rectangle{briefPosition, textAreaSize};
			const auto textPosition = briefPosition + textPadding;

			renderer.drawBoxFilled(notificationBriefRect, Color::DarkGray);
			renderer.drawBox(notificationBriefRect, Color::Black);
			renderer.drawText(mFont, notification.brief, textPosition, Color::White);
		}

		count++;
	}
}
