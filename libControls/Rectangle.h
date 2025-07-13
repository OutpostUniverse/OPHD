#pragma once

#include "Control.h"

#include <NAS2D/Renderer/Color.h>


class Rectangle : public Control
{
public:
	Rectangle(NAS2D::Color color, NAS2D::Vector<int> size = {1, 1});

	void draw(NAS2D::Renderer& renderer) const override;

private:
	NAS2D::Color mColor;
};
