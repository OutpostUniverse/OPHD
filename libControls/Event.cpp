#include "Event.h"

#include "Control.h"

namespace Event
{
	void dispatchEvent(Event& event, std::vector<Control*> controls)
	{
		for (auto& control : controls)
		{
			control->handleEvent(event);
		}
	}
}
