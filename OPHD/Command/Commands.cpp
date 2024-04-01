#include "Command.h"

class MoveCommand : public Command
{
public:
	MoveCommand(MapView& mapView, NAS2D::Vector<int> vector, int* scalarFactor) :
		mMapView(mapView),
		mVector(vector),
		mScalarFactor(scalarFactor)
	{}

	virtual void execute() override
	{
		mMapView.moveView(MapOffset{mVector * (*mScalarFactor), 0});
	}

private:
	MapView& mMapView;
	NAS2D::Vector<int> mVector;
	int* mScalarFactor;

class NullCommand : public Command
{
public:
    virtual void execute() override
    {
        // Do nothing
    }
};
