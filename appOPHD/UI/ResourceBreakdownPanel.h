#pragma once

#include <libOPHD/StorableResources.h>

#include <libControls/Control.h>

#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Renderer/RectangleSkin.h>


class ResourceBreakdownPanel : public Control
{
public:
	ResourceBreakdownPanel();

	void playerResources(const StorableResources* resources) { mPlayerResources = resources; }
	void previousResources(const StorableResources& resources) { mPreviousResources = resources; }
	StorableResources& previousResources() { return mPreviousResources; }
	void draw(NAS2D::Renderer& renderer) const override;

private:
	const NAS2D::Font& mFont;
	const NAS2D::Image& mIcons;
	NAS2D::RectangleSkin mSkin;

	StorableResources mPreviousResources;
	const StorableResources* mPlayerResources = nullptr;
};
