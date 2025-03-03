#pragma once

#include "Control.h"
#include "Event.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/EventHandler.h>

#include <vector>


/**
 * UI Object that contains other UI Controls.
 *
 * Generally not intended to be used by itself.
 */
class UIContainer : public Control
{
public:
	virtual void dispatchEvent(Event::Event& event) { Event::dispatchEvent(event, mControls); }
protected:
	UIContainer();
	UIContainer(std::vector<Control*> controls);
	~UIContainer() override;

	void add(Control& control, NAS2D::Vector<int> offset);
	void clear();

	void bringToFront(Control* control);

	void update() override;

	const std::vector<Control*>& controls() const;

protected:
	void onVisibilityChange(bool visible) override;
	void onMove(NAS2D::Vector<int> displacement) override;

	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);

private:
	std::vector<Control*> mControls;
};
