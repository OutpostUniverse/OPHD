#pragma once

#include <vector>
#include <functional>

class Control;
class UIContainer;

namespace ContainerPolicy
{
	using ContainerPolicyFunction = std::function<void(std::vector<Control*>&)>;
	void layoutVertical(std::vector<Control*>& controls);
	ContainerPolicyFunction rightJustify(Control& controlsToJustify, UIContainer& container);
}
