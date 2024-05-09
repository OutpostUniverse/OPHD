#pragma once

#include <NAS2D/Math/Vector.h>


struct MapOffset
{
	NAS2D::Vector<int> xy;
	int z;
};


inline bool operator==(const MapOffset& offset1, const MapOffset& offset2)
{
	return offset1.xy == offset2.xy && offset1.z == offset2.z;
}


inline bool operator!=(const MapOffset& offset1, const MapOffset& offset2)
{
	return !(offset1 == offset2);
}


inline MapOffset& operator*=(MapOffset& offset, int scalar)
{
	offset.xy *= scalar;
	offset.z *= scalar;
	return offset;
}


inline MapOffset operator*(MapOffset offset, int scalar)
{
	return offset *= scalar;
}


inline MapOffset operator*(int scalar, MapOffset offset)
{
	return offset *= scalar;
}
