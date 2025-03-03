#pragma once

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/EnumKeyModifier.h>

#include <vector>

class Control;
namespace Event
{
	struct Event
	{
		enum class Type
		{
			None,
			KeyDown
		};

		Type type = Type::None;
		NAS2D::KeyCode keyCode = NAS2D::KeyCode::Unknown;
		NAS2D::KeyModifier keyMod = NAS2D::KeyModifier::None;
		bool repeat = false;
		bool handled = false;
	};

	void dispatchEvent(Event&, std::vector<Control*>);
}
