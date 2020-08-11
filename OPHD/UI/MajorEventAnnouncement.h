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

public:
	MajorEventAnnouncement();
	~MajorEventAnnouncement() override;

	void announcement(AnnouncementType a);

	void update() override;

protected:
	virtual void init();

private:
	void btnCloseClicked();

private:
	MajorEventAnnouncement(const MajorEventAnnouncement&) = delete;
	MajorEventAnnouncement& operator=(const MajorEventAnnouncement&) = delete;

private:
	const NAS2D::Image& mHeader;

	std::string mMessage;

	Button btnClose{"Okay"};
};
