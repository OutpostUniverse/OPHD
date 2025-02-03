#pragma once

#include <string>


namespace NAS2D
{
	template <typename BaseType>
	struct Vector;
}


enum PointerType
{
	POINTER_NORMAL,
	POINTER_PLACE_TILE
};


void setCursor(PointerType pointerType);
void addCursor(PointerType pointerType, const std::string& fileName, NAS2D::Vector<int> hotOffset);
