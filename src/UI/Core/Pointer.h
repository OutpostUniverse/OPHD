#ifndef __POINTER__
#define __POINTER__

#include "NAS2D/NAS2D.h"
#include <sstream>

class Pointer
{
public:

	Pointer(const std::string& image_path, int offsetX, int offsetY):	mPointerImage(image_path),
																		mOffset(offsetX, offsetY)
	{}

	Pointer(const Pointer& p):	mPointerImage(p.mPointerImage),
								mOffset(p.mOffset)
	{}

	void draw(int x, int y)
	{
		Utility<Renderer>::get().drawImage(mPointerImage, static_cast<float>(x - mOffset.x()), static_cast<float>(y - mOffset.y()));
	}

private:

	NAS2D::Image		mPointerImage;
	NAS2D::Point_2d		mOffset;
};

#endif