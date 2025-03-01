#pragma once

#include "Control.h"

#include <vector>
#include <functional>

namespace ContainerPolicy
{
	using ContainerPolicyFunction = std::function<void(std::vector<Control*>&)>;
	extern ContainerPolicyFunction layoutVertical;
}
