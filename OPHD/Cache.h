#pragma once

#include <NAS2D/Resources/Image.h>
#include <NAS2D/Resources/Music.h>
#include <NAS2D/Resources/ResourceCache.h>

#include <memory>


inline ResourceCache<NAS2D::Image, std::string> imageCache;

inline std::unique_ptr<NAS2D::Music> trackMars;
