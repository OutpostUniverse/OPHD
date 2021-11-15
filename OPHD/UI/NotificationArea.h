#pragma once

#include "Core/Control.h"

#include "../Map/MapCoordinate.h"

#include <vector>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Signal/Signal.h>


class NotificationArea : public Control
{
public:
	enum class NotificationType
	{
		Critical,
		Information,
		Warning
	};


	struct Notification
	{
		std::string brief{""};
		std::string message{""};
		MapCoordinate position{{-1, -1}, 0};
		NotificationType type{NotificationType::Information};
	};

	const int Width = 48;

	using NotificationCallback = NAS2D::Signal<const Notification&>;

public:
	NotificationArea();
	~NotificationArea() override;

	void push(const std::string& brief, const std::string& message, const MapCoordinate& position, NotificationType type);

	void clear()
	{
		mNotificationList.clear();
	}

	NotificationCallback& notificationClicked() { return mNotificationClicked; }

	const Notification& notificationFromIndex(int index)
	{
		return mNotificationList.at(static_cast<size_t>(index));
	}

	void update() override;

protected:
	NAS2D::Rectangle<int> notificationRect(std::size_t count);

	void onMouseDown(NAS2D::EventHandler::MouseButton, int, int);
	void onMouseMove(int x, int y, int dX, int dY);

private:
	const NAS2D::Image& mIcons;
	const NAS2D::Font& mFont;

	std::vector<Notification> mNotificationList;

	std::size_t mNotificationIndex{SIZE_MAX};
	NAS2D::Rectangle<int> mNotificationBriefRect{0};

	NotificationCallback mNotificationClicked;
};

const NAS2D::Rectangle<float>& IconRectFromNotificationType(const NotificationArea::NotificationType);
const NAS2D::Color ColorFromNotification(const NotificationArea::NotificationType);
const std::string& StringFromNotificationType(const NotificationArea::NotificationType);
