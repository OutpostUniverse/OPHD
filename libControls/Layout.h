#pragma once

#include <vector>


namespace NAS2D
{
	template <typename BaseType> struct Point;
	template <typename BaseType> struct Vector;
}


class Control;


NAS2D::Vector<int> maxSize(const std::vector<Control*>& controls);
void setSize(const std::vector<Control*>& controls, NAS2D::Vector<int> newSize);
