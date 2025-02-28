#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>


class MajorEventAnnouncement : public Window
{
public:

	enum class AnnouncementType
	{
		ColonyShipCrash,
		ColonyShipCrashWithColonists,
		ColonyShipCrashWithCargo,
		ColonyShipCrashWithColonistsAndCargo,
	};

	MajorEventAnnouncement();

	void announcement(AnnouncementType a);

	void update() override;

private:
	void onClose();

	const NAS2D::Image& mHeader;
	std::string mMessage;
	Button btnClose{"Okay", {this, &MajorEventAnnouncement::onClose}};
};
