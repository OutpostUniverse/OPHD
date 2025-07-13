#pragma once

#include <libControls/Control.h>

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Resource/Image.h>


class PlanetImage : public Control
{
public:
	using MouseEventDelegate = NAS2D::Delegate<void()>;

public:
	PlanetImage(const std::string& imagePath);
	PlanetImage(const PlanetImage&) = delete;
	PlanetImage& operator=(const PlanetImage&) = delete;
	~PlanetImage() override;

	bool isMouseOver() const { return mIsMouseOver; }

	void mouseEnterHandler(MouseEventDelegate newMouseEnterHandler);
	void mouseExitHandler(MouseEventDelegate newMouseExitHandler);

	void update() override;
	void draw(NAS2D::Renderer& renderer) const override;

protected:
	bool pointInCircle(NAS2D::Point<int> point) const;
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

private:
	const NAS2D::Image mImage;
	NAS2D::Timer mTimer;

	bool mIsMouseOver = false;
	int mFrameIndex = 0;

	MouseEventDelegate mMouseEnterHandler;
	MouseEventDelegate mMouseExitHandler;
};
