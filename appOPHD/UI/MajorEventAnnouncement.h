#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>


class WindowStack;
struct ColonyShipLanders;


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

	void announcement(AnnouncementType announcementType);

	void onColonyShipCrash(WindowStack& windowStack, const ColonyShipLanders& colonyShipLanders);

	void drawClientArea(NAS2D::Renderer& renderer) const override;

protected:
	void onClose();

	AnnouncementType colonyShipCrashAnnouncement(const ColonyShipLanders& colonyShipLanders);

private:
	const NAS2D::Image& mHeader;
	std::string mMessage;
	Button btnClose;
};
