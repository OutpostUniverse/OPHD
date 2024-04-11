#pragma once

#include "Command.h"
#include <NAS2D/EventHandler.h>
#include <NAS2D/Signal/Signal.h>
#include "../Map/MapView.h"
#include <OPHD/UI/CheatMenu.h>
#include <libControls/WindowStack.h>

class MoveCommand : public Command
{
public:
	MoveCommand(MapView& mapView, NAS2D::Vector<int> vector, int* mMoveCommandScalar) :
		mMapView(mapView),
		mVector(vector),
		mMoveCommandScalar_(mMoveCommandScalar)
	{}

	virtual void execute() override
	{
		mMapView.moveView(MapOffset{mVector * (*mMoveCommandScalar_), 0});
	}

private:
	MapView& mMapView;
	NAS2D::Vector<int> mVector;
	int* mMoveCommandScalar_ = nullptr;
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

class ShowCheatMenuCommand : public Command
{
public:
	ShowCheatMenuCommand(CheatMenu* cheatMenu, WindowStack* windowStack) :
		mCheatMenu(cheatMenu),
		mWindowStack(windowStack)
	{}

	virtual void execute() override
	{
		mCheatMenu->show();
		mWindowStack->bringToFront(mCheatMenu);
	}
	private:
	CheatMenu* mCheatMenu;
	WindowStack* mWindowStack;
};

class NullCommand : public Command
{
public:
	virtual void execute() override
	{
		// Do nothing
	}
};
