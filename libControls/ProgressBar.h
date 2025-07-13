#pragma once

#include "Control.h"


class ProgressBar : public Control
{
public:
	ProgressBar(int max, int value = 0, int padding = 4);

	void draw(NAS2D::Renderer& renderer) const override;

private:
	const int mPadding;
	const int mMax;
	int mValue;
};
