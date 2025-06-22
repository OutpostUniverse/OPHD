#pragma once

#include "Report.h"

#include <NAS2D/Signal/Delegate.h>


namespace NAS2D
{
	class Font;
	class Image;
}


class SatellitesReport : public Report
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;

	SatellitesReport(TakeMeThereDelegate takeMeThereHandler);
	~SatellitesReport() override;

	void fillLists() override;
	void clearSelected() override;

	void refresh() override;
	void selectStructure(Structure*) override;

	void update() override;

private:
	void onResize() override;
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position) override;

	void draw() const override;

	TakeMeThereDelegate mTakeMeThereHandler;
	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;

	const NAS2D::Image& imageNotImplemented;
};
