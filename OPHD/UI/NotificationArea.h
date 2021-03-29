#pragma once

#include "Core/Control.h"

#include <vector>
#include <NAS2D/EventHandler.h>

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
		std::string message;
		NotificationType type;
		NAS2D::Point<int> position{ 0, 0 };
	};

public:
	NotificationArea();
	~NotificationArea() override;

	void push(Notification);

	void clear() { mNotificationList.clear(); }

	void update() override;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int, int);
	void onMouseMove(int, int, int, int);

private:
	std::vector<Notification> mNotificationList;
};
