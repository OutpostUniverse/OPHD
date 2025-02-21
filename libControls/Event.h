#pragma once

#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/EnumKeyModifier.h>

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
};
