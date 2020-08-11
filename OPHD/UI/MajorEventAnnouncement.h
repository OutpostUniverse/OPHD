#pragma once

#include "Core/Window.h"
#include "Core/Button.h"


class MajorEventAnnouncement : public Window
{
public:

	enum AnnouncementType
	{
		ANNOUNCEMENT_COLONY_SHIP_CRASH,
		ANNOUNCEMENT_COLONY_SHIP_CRASH_WITH_COLONISTS
	};

	MajorEventAnnouncement();
	MajorEventAnnouncement(const MajorEventAnnouncement&) = delete;
	MajorEventAnnouncement& operator=(const MajorEventAnnouncement&) = delete;

	void announcement(AnnouncementType a);

	void update() override;

protected:
	virtual void init();

private:
	void btnCloseClicked();

	const NAS2D::Image& mHeader;
	std::string mMessage;
	Button btnClose{"Okay"};
};
