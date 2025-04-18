#pragma once

#include "ReportInterface.h"


namespace NAS2D
{
	class Font;
	class Image;
}


class SpaceportsReport : public ReportInterface
{
public:
	SpaceportsReport();
	~SpaceportsReport() override;

	void fillLists() override;
	void clearSelected() override;

	void refresh() override;
	void selectStructure(Structure*) override;

	void update() override;

private:
	void onResize() override;
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position) override;

	void draw() const override;

	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;

	const NAS2D::Image& imageNotImplemented;
};
