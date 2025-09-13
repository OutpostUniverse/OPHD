#pragma once

#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/ResourceCache.h>


inline NAS2D::ResourceCache<NAS2D::Image, std::string> imageCache;


const NAS2D::Image& getImage(const std::string& imageName);
