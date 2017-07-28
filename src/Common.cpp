#include "Common.h"

#include <NAS2D/NAS2D.h>

using namespace NAS2D;

/**
 * Convenience function to pass a Rectangle_2df to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, const Rectangle_2df& rect)
{
	return pointInRect_f(x, y, rect.x(), rect.y(), rect.width(), rect.height());
}


/**
 * Convenience function to pass a \c float's to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, float rectX, float rectY, float rectW, float rectH)
{
	return isPointInRect(x, y,	static_cast<int>(rectX), static_cast<int>(rectY), static_cast<int>(rectW), static_cast<int>(rectH));
}