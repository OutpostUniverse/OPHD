#pragma once

#include "Control.h"


namespace NAS2D
{
	class Image;
}


class Image : public Control
{
public:
	Image(const NAS2D::Image& image);

	void draw(NAS2D::Renderer& renderer) const override;

private:
	const NAS2D::Image& mImage;
};
