#include "CacheImage.h"


namespace
{
	NAS2D::ResourceCache<NAS2D::Image, std::string> imageCache;
}


NAS2D::ResourceCache<NAS2D::Image, std::string>& getImageCache()
{
	return imageCache;
}


const NAS2D::Image& getImage(const std::string& imageName)
{
	return imageCache.load(imageName);
}
