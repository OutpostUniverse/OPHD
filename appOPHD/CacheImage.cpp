#include "CacheImage.h"

#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/AnimationSet.h>
#include <NAS2D/Resource/ResourceCache.h>


namespace
{
	NAS2D::ResourceCache<NAS2D::Image, std::string> imageCache;
	NAS2D::ResourceCache<NAS2D::AnimationSet, std::string> animationSetCache;
}


NAS2D::ResourceCache<NAS2D::Image, std::string>& getImageCache()
{
	return imageCache;
}


const NAS2D::Image& getImage(const std::string& imageName)
{
	return imageCache.load(imageName);
}


const NAS2D::AnimationSet& getAnimationSet(const std::string& filePath)
{
	return animationSetCache.load(filePath);
}
