#pragma once

#include "NAS2D/Resources/Image.h"

#include "Core/Control.h"
#include "../ResourcePool.h"

class ResourceBreakdownPanel : public Control
{
public:
	ResourceBreakdownPanel();

	void playerResources(ResourcePool* rp) { mPlayerResources = rp; }
	ResourcePool& previousResources() { return mPreviousResources; }

	void resourceCheck();

	void update() override;

private:
	NAS2D::Image mIcons;
	NAS2D::ImageList mSkin;

	ResourcePool mPreviousResources;
	ResourcePool* mPlayerResources = nullptr;
};
