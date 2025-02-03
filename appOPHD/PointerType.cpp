#include "PointerType.h"

#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Utility.h>


void setCursor(PointerType pointerType)
{
	NAS2D::Utility<NAS2D::Renderer>::get().setCursor(pointerType);
}


void addCursor(PointerType pointerType, const std::string& fileName, NAS2D::Vector<int> hotOffset)
{
	NAS2D::Utility<NAS2D::Renderer>::get().addCursor(fileName, pointerType, hotOffset.x, hotOffset.y);
}
