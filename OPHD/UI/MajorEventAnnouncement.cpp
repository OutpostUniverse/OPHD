// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MajorEventAnnouncement.h"

#include "../Constants.h"
#include "../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"

using namespace NAS2D;

MajorEventAnnouncement::MajorEventAnnouncement() :
	btnClose{"Okay"}
{
	init();
}


MajorEventAnnouncement::~MajorEventAnnouncement()
{}


void MajorEventAnnouncement::init()
{
	position(0, 0);
	size({522, 340});

	add(&btnClose, 5, 310);
	btnClose.size({512, 25});
	btnClose.click().connect(this, &MajorEventAnnouncement::btnCloseClicked);

	anchored(true);
}


void MajorEventAnnouncement::btnCloseClicked()
{
	hide();
}


void MajorEventAnnouncement::announcement(AnnouncementType a)
{
	switch (a)
	{
	case AnnouncementType::ANNOUNCEMENT_COLONY_SHIP_CRASH:
		mHeader = Image("ui/interface/colony_ship_crash.png");
		mMessage = "Colony ship deorbited and crashed on the surface.";
		break;
	case AnnouncementType::ANNOUNCEMENT_COLONY_SHIP_CRASH_WITH_COLONISTS:
		mHeader = Image("ui/interface/colony_ship_crash.png");
		mMessage = "Colony ship deorbited and crashed on the surface but you left colonists on board!";
		break;
	default:
		throw std::runtime_error("MajorEventAnnouncement::announcement(): Called with invalid announcement type.");
		break;
	}
}


void MajorEventAnnouncement::update()
{
	if (!visible()) { return; }

	Window::update();

	Renderer& r = Utility<Renderer>::get();

	r.drawImage(mHeader, rect().x() + 5, rect().y() + 25);

	// Yeah, I know. I hate it too but it made more sense than holding onto a static pointer.
	const auto& font = *Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	r.drawText(font, mMessage, rect().x() + 5, rect().y() + 290, 255, 255, 255);
}
