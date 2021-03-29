#pragma once

#include "Core/Control.h"


#include <vector>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Resources/Image.h>


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
		std::string message{ "" };
		NotificationType type{ NotificationType::Information };
		NAS2D::Point<int> position{ 0, 0 };
		NAS2D::Vector<int> size{ 32, 32 };
	};

public:
	NotificationArea();
	~NotificationArea() override;

	void push(const std::string& message, NotificationType type);

	void clear() { mNotificationList.clear(); }

	void update() override;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int, int);

private:
	const NAS2D::Image& mIcons;
	std::vector<Notification> mNotificationList;
};
