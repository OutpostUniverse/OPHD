#include "Image.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Image.h>


Image::Image(const NAS2D::Image& image) :
	mImage{image}
{
	size(mImage.size());
}


void Image::draw() const
{
	if (!visible()) { return; }

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	renderer.drawImage(mImage, mRect.position);
}
