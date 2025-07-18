#include "Image.h"

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Image.h>


Image::Image(const NAS2D::Image& image) :
	mImage{image}
{
	size(mImage.size());
}


void Image::draw(NAS2D::Renderer& renderer) const
{
	if (!visible()) { return; }

	renderer.drawImage(mImage, mRect.position);
}
