#pragma once

#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Music.h>
#include <NAS2D/Resource/ResourceCache.h>

#include <memory>


inline NAS2D::ResourceCache<NAS2D::Font, std::string, unsigned int> fontCache;
inline NAS2D::ResourceCache<NAS2D::Image, std::string> imageCache;

inline std::unique_ptr<NAS2D::Music> trackMars;
