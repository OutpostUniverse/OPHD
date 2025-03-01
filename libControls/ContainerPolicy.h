#pragma once

#include <vector>
#include <functional>

namespace ContainerPolicy
{
	using ContainerPolicyFunction = std::function<void(std::vector<Control*>&)>;
	void layoutVertical(std::vector<Control*>& controls);
}
