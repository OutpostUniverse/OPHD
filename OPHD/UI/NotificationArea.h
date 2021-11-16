#pragma once

#include "Core/Control.h"

#include "../Map/MapCoordinate.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Signal/Signal.h>

#include <vector>
#include <string>
#include <cstddef>


namespace NAS2D
{
	class Image;
	class Font;
}


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

	using NotificationClickedSignal = NAS2D::Signal<const Notification&>;

public:
	NotificationArea();
	~NotificationArea() override;

	void push(Notification notification);
	void clear();

	NotificationClickedSignal::Source& notificationClicked() { return mNotificationClicked; }

	void update() override;

protected:
	NAS2D::Rectangle<int> notificationRect(std::size_t index);
	std::size_t notificationIndex(NAS2D::Point<int> pixelPosition);

	void onMouseDown(NAS2D::EventHandler::MouseButton, int, int);
	void onMouseMove(int x, int y, int dX, int dY);

private:
	const NAS2D::Image& mIcons;
	const NAS2D::Font& mFont;

	std::vector<Notification> mNotificationList;
	std::size_t mNotificationIndex;
	NotificationClickedSignal mNotificationClicked;
};

void drawNotificationIcon(NAS2D::Point<int> position, NotificationArea::NotificationType type, const NAS2D::Image& icons);
