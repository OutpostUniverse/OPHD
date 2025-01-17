#pragma once

#include <NAS2D/Signal/Signal.h>

class Deployable
{
public:
	using SignalSource = NAS2D::SignalSource<>;
	using Signal = NAS2D::Signal<>;
	virtual ~Deployable() = default;
	virtual SignalSource& deploySignal() = 0;

protected:
	virtual void bulldoze() = 0;
};
