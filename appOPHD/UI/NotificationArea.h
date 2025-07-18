#pragma once

#include <libOPHD/Map/MapCoordinate.h>

#include <libControls/Control.h>

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Signal/Delegate.h>

#include <vector>
#include <string>
#include <cstddef>


namespace NAS2D
{
	enum class MouseButton;

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
		Success,
		Warning
	};


	struct Notification
	{
		std::string brief{};
		std::string message{};
		MapCoordinate position{{-1, -1}, 0};
		NotificationType type{NotificationType::Information};

		bool hasMapCoordinate() const
		{
			return position.xy != NAS2D::Point{-1, -1};
		}
	};

	using NotificationClickedDelegate = NAS2D::Delegate<void(const Notification&)>;

public:
	NotificationArea(NotificationClickedDelegate notificationClickedHandler);
	~NotificationArea() override;

	void push(Notification notification);
	void clear();

	void draw(NAS2D::Renderer& renderer) const override;

protected:
	NAS2D::Rectangle<int> notificationRect(std::size_t index) const;
	std::size_t notificationIndex(NAS2D::Point<int> pixelPosition);

	void onMouseDown(NAS2D::MouseButton, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

private:
	const NAS2D::Image& mIcons;
	const NAS2D::Font& mFont;

	std::vector<Notification> mNotificationList;
	std::size_t mNotificationIndex;
	NotificationClickedDelegate mNotificationClickedHandler;
};

void drawNotificationIcon(NAS2D::Renderer& renderer, NAS2D::Point<int> position, NotificationArea::NotificationType type, const NAS2D::Image& icons);
