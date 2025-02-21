#pragma once

#include "../../libControls/Control.h"

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/EnumKeyModifier.h>

class MapControl : public Control
{
public:
	MapControl();
	~MapControl() override;

private:
	void onKeyDownEvent(NAS2D::KeyCode keyCode, NAS2D::KeyModifier keyMod, bool repeat);
};
