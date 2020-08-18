#pragma once

#include "Core/Control.h"
#include "../StorableResources.h"

#include <NAS2D/Resources/Font.h>
#include <NAS2D/Resources/Image.h>
#include <NAS2D/Renderer/RectangleSkin.h>


class ResourceBreakdownPanel : public Control
{
public:
	ResourceBreakdownPanel();

	void playerResources(StorableResources* resources) { mPlayerResources = resources; }
	void previousResources(StorableResources& resources) { mPreviousResources = resources; }

	StorableResources& previousResources() { return mPreviousResources; }

	void resourceCheck();

	void update() override;

private:
	const NAS2D::Image& mIcons;
	NAS2D::RectangleSkin mSkin;

	StorableResources mPreviousResources;
	StorableResources* mPlayerResources = nullptr;
};
