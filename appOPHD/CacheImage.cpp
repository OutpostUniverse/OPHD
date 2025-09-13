#include "CacheImage.h"


const NAS2D::Image& getImage(const std::string& imageName)
{
	return imageCache.load(imageName);
}
