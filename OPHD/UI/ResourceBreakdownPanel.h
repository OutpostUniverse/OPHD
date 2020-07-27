#pragma once

#include "Core/Control.h"
#include "../StorableResources.h"

#include <NAS2D/Resources/Image.h>


class ResourceBreakdownPanel : public Control
{
public:
	ResourceBreakdownPanel();

	void playerResources(StorableResources* resourcePool) { mPlayerResources = resourcePool; }
	StorableResources& previousResources() { return mPreviousResources; }

	void resourceCheck();

	void update() override;

private:
	NAS2D::Image mIcons;
	NAS2D::ImageList mSkin;

	StorableResources mPreviousResources;
	StorableResources* mPlayerResources = nullptr;
};
