#pragma once

#include <string>


namespace NAS2D
{
	template <typename BaseType>
	struct Vector;

	struct CursorId;
}


enum class PointerType
{
	Normal,
	PlaceTile,
};


constexpr NAS2D::CursorId pointerTypeToCursorId(PointerType pointerType);

void setCursor(PointerType pointerType);
void addCursor(PointerType pointerType, const std::string& fileName, NAS2D::Vector<int> hotOffset);
