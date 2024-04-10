#include "Command.h"
#include <NAS2D/EventHandler.h>
#include <NAS2D/Signal/Signal.h>
#include "../Map/MapView.h"

class MoveCommand : public Command
{
public:
	MoveCommand(MapView& mapView, NAS2D::Vector<int> vector, int scalarFactor) :
		mMapView(mapView),
		mVector(vector),
		mScalarFactor(scalarFactor)
	{}

	virtual void execute() override
	{
		mMapView.moveView(MapOffset{mVector * mScalarFactor, 0});
	}

private:
	MapView& mMapView;
	NAS2D::Vector<int> mVector;
	int mScalarFactor;
};

class SignalCommand : public Command
{
public:
	SignalCommand(NAS2D::Signal<>* signal) :
		mSignal(signal)
	{}

	virtual void execute() override
	{
		mSignal->emit();
	}

private:
	NAS2D::Signal<>* mSignal;
};

class NullCommand : public Command
{
public:
	virtual void execute() override
	{
		// Do nothing
	}
};
