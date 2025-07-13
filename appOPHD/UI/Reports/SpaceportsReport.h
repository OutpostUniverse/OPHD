#pragma once

#include "Report.h"

#include <NAS2D/Signal/Delegate.h>


namespace NAS2D
{
	class Font;
	class Image;
}


class SpaceportsReport : public Report
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;

	SpaceportsReport(TakeMeThereDelegate takeMeThereHandler);
	~SpaceportsReport() override;

	bool canView(const Structure& structure) override;
	void selectStructure(Structure&) override;
	void clearSelected() override;
	void fillLists() override;
	void refresh() override;

	void update() override;

private:
	void onResize() override;
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position) override;

	void draw(NAS2D::Renderer& renderer) const override;

	TakeMeThereDelegate mTakeMeThereHandler;
	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;

	const NAS2D::Image& imageNotImplemented;
};
