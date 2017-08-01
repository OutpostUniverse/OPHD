#pragma once

#include "UI.h"

class MajorEventAnnouncement : public Window
{
public:

	enum AnnouncementType
	{
		ANNOUNCEMENT_COLONY_SHIP_CRASH,
		ANNOUNCEMENT_COLONY_SHIP_CRASH_WITH_COLONISTS
	};

public:

	MajorEventAnnouncement(Font& font);
	virtual ~MajorEventAnnouncement();

	void announcement(AnnouncementType a);

	virtual void update();

protected:

	virtual void init();

private:

	void btnCloseClicked();

	MajorEventAnnouncement();
	MajorEventAnnouncement(const MajorEventAnnouncement&);
	MajorEventAnnouncement& operator=(const MajorEventAnnouncement&);

	Image			mHeader;

	std::string		mMessage;

	Button			btnClose;
};
