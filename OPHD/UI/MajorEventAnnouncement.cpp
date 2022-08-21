#include "MajorEventAnnouncement.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
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


void MajorEventAnnouncement::announcement(AnnouncementType a)
{
	switch (a)
	{
	case AnnouncementType::ANNOUNCEMENT_COLONY_SHIP_CRASH:
		mMessage = "Colony ship deorbited and crashed on the surface.";
		break;
	case AnnouncementType::ANNOUNCEMENT_COLONY_SHIP_CRASH_WITH_COLONISTS:
		mMessage = "Colony ship deorbited and crashed on the surface but you left colonists on board!";
		break;
	}
}


void MajorEventAnnouncement::update()
{
	if (!visible()) { return; }

	Window::update();

	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(mHeader, position() + NAS2D::Vector{5, 25});

	const auto& font = fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal);
	renderer.drawText(font, mMessage, position() + NAS2D::Vector{5, 290}, NAS2D::Color::White);
}
