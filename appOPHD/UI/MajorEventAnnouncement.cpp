#include "MajorEventAnnouncement.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"
#include "../States/ColonyShip.h"

#include <libControls/WindowStack.h>

#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


MajorEventAnnouncement::MajorEventAnnouncement() :
	mHeader{imageCache.load("ui/interface/colony_ship_crash.png")}
{
	position({0, 0});
	size({522, 340});

	add(btnClose, {5, 310});
	btnClose.size({512, 25});

	anchored(true);
}


void MajorEventAnnouncement::onClose()
{
	hide();
}


MajorEventAnnouncement::AnnouncementType MajorEventAnnouncement::colonyShipCrashAnnouncement(const ColonyShipLanders& colonyShipLanders)
{
	if (colonyShipLanders.colonist && colonyShipLanders.cargo)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithColonistsAndCargo;

	if (colonyShipLanders.colonist)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithColonists;

	if (colonyShipLanders.cargo)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithCargo;

	return MajorEventAnnouncement::AnnouncementType::ColonyShipCrash;
}


void MajorEventAnnouncement::announcement(AnnouncementType a)
{
	switch (a)
	{
	case AnnouncementType::ColonyShipCrash:
		mMessage = "Colony ship deorbited and crashed.";
		break;
	case AnnouncementType::ColonyShipCrashWithColonists:
		mMessage = "Colony ship deorbited and crashed with colonists on board!";
		break;
	case AnnouncementType::ColonyShipCrashWithCargo:
		mMessage = "Colony ship deorbited and crashed with cargo on board!";
		break;
	case AnnouncementType::ColonyShipCrashWithColonistsAndCargo:
		mMessage = "Colony ship deorbited and crashed with colonists and cargo on board!";
		break;
	}
}


void MajorEventAnnouncement::onColonyShipCrash(WindowStack& windowStack, const ColonyShipLanders& colonyShipLanders)
{
	windowStack.bringToFront(*this);
	announcement(colonyShipCrashAnnouncement(colonyShipLanders));
	show();
}


void MajorEventAnnouncement::drawClientArea(NAS2D::Renderer& renderer) const
{
	renderer.drawImage(mHeader, position() + NAS2D::Vector{5, 25});

	const auto& font = Control::getDefaultFont();
	renderer.drawText(font, mMessage, position() + NAS2D::Vector{5, 290}, NAS2D::Color::White);
}
