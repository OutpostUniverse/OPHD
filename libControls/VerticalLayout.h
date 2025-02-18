#pragma once

#include "UIContainer.h"

#include<NAS2D/Math/Vector.h>

#include <unordered_map>

class VerticalLayout : public UIContainer
{
public:
	VerticalLayout();
	VerticalLayout(std::vector<Control*> controls, NAS2D::Vector<int> offset = {0, 0});
	void add(Control& control, NAS2D::Vector<int> offset);
	void addRow(int size) { nextVerticalPosition += size; }
	void padding(int padding) { this->paddingBottom = padding; this->paddingTop = padding; };
	NAS2D::Vector<int> minimumSize() const { return {minimumHorizontalSize, nextVerticalPosition}; }


private:
	NAS2D::Vector<int> arrangeControl(Control& control, NAS2D::Vector<int> offset);
	int nextVerticalPosition = 0;
	int minimumHorizontalSize = 0;
	int paddingTop = 2;
	int paddingBottom = 2;
};
