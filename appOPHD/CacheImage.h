#pragma once

#include <string>


namespace NAS2D
{
	class Image;
	template <typename Resource, typename... Params> class ResourceCache;
}


NAS2D::ResourceCache<NAS2D::Image, std::string>& getImageCache();

const NAS2D::Image& getImage(const std::string& imageName);
