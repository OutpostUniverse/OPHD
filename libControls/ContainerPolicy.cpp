#include "ContainerPolicy.h"
#include "Control.h"
#include "UIContainer.h"

namespace ContainerPolicy
{
	void layoutVertical(std::vector<Control*>& controls) 
	{
		int y = 0;
		for (auto& control : controls)
		{
			control->position({0, y});
			y += control->size().y;
		}
	}

	ContainerPolicyFunction rightJustify(Control& controlToJustify, UIContainer& container)
	{
		return [&controlToJustify, &container](std::vector<Control*>& controls) {
			for (auto& control : controls)
			{
				if(control == &controlToJustify)
				{
					control->position({container.size().x - control->size().x, control->position().y});
				}

			}
		};
	}
	ContainerPolicyFunction rightJustify(std::vector<Control*>& controlsToJustify, UIContainer& container)
	{
		return [&controlsToJustify, &container](std::vector<Control*>& controls) {
			for (auto& control : controls)
			{
				if(std::find(controlsToJustify.begin(), controlsToJustify.end(), control) != controlsToJustify.end())
				{
					control->position({container.size().x - control->size().x, control->position().y});
				}
			}
		};
	}
}
