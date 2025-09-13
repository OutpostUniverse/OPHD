#pragma once

#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/ResourceCache.h>


NAS2D::ResourceCache<NAS2D::Image, std::string>& getImageCache();

const NAS2D::Image& getImage(const std::string& imageName);
