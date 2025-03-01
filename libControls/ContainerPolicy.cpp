#include "ContainerPolicy.h"
#include "Control.h"

namespace ContainerPolicy
{
	ContainerPolicy::ContainerPolicyFunction layoutVertical = [](std::vector<Control*>& controls) {
		int y = 0;
		for (auto& control : controls)
		{
			control->position({0, y});
			y += control->size().y;
		}
	};
}
