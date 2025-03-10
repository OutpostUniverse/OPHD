#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>

class WindowStack;
struct ColonyShipData;

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

	void onColonyShipCrash(WindowStack&, const ColonyShipData&);

	void update() override;

private:
	void onClose();

	MajorEventAnnouncement::AnnouncementType colonyShipCrashAnnouncement(const ColonyShipData&);

	const NAS2D::Image& mHeader;
	std::string mMessage;
	Button btnClose{"Okay", {this, &MajorEventAnnouncement::onClose}};
};
