#include "PointerType.h"

#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Utility.h>


constexpr NAS2D::CursorId pointerTypeToCursorId(PointerType pointerType)
{
	return NAS2D::CursorId{static_cast<int>(pointerType)};
}


void setCursor(PointerType pointerType)
{
	NAS2D::Utility<NAS2D::Renderer>::get().setCursor(pointerTypeToCursorId(pointerType));
}


void addCursor(PointerType pointerType, const std::string& fileName, NAS2D::Vector<int> hotOffset)
{
	NAS2D::Utility<NAS2D::Renderer>::get().addCursor(pointerTypeToCursorId(pointerType), fileName, hotOffset);
}
