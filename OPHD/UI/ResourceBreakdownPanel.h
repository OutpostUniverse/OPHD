#pragma once

#include "Core/Control.h"
#include "../ResourcePool.h"

#include <NAS2D/Resources/Image.h>


class ResourceBreakdownPanel : public Control
{
public:
	ResourceBreakdownPanel();

	void playerResources(ResourcePool* resourcePool) { mPlayerResources = resourcePool; }
	ResourcePool& previousResources() { return mPreviousResources; }

	void resourceCheck();

	void update() override;

private:
	const NAS2D::Image& mIcons;
	NAS2D::ImageList mSkin;

	ResourcePool mPreviousResources;
	ResourcePool* mPlayerResources = nullptr;
};
