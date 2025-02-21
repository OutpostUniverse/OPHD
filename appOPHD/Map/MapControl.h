#pragma once

#include <libControls/Control.h>

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/EnumKeyModifier.h>

class CheatMenu;
class WindowStack;
class MapControl : public Control
{
public:
	MapControl(CheatMenu& cheatMenu, WindowStack& windowStack);
	~MapControl() override;
	virtual void handleEvent(Event& event) override;

private:
	void onKeyDownEvent(NAS2D::KeyCode keyCode, NAS2D::KeyModifier keyMod, bool repeat);
	void onKeyDown(Event& event);
	CheatMenu& mCheatMenu;
	WindowStack& mWindowStack;
};
