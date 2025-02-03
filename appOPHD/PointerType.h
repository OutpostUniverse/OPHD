#pragma once

#include <string>


namespace NAS2D
{
	template <typename BaseType>
	struct Vector;
}


enum class PointerType
{
	Normal,
	PlaceTile,
};


void setCursor(PointerType pointerType);
void addCursor(PointerType pointerType, const std::string& fileName, NAS2D::Vector<int> hotOffset);
