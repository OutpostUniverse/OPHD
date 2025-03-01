#include "Layout.h"

#include "Control.h"

#include <algorithm>


NAS2D::Vector<int> maxSize(const std::vector<Control*>& controls)
{
	NAS2D::Vector<int> maxSize{0, 0};
	for (const auto* control : controls)
	{
		const auto controlSize = control->size();
		maxSize.x = std::max(maxSize.x, controlSize.x);
		maxSize.y = std::max(maxSize.y, controlSize.y);
	}
	return maxSize;
}
