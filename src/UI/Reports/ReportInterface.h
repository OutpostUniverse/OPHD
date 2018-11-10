#pragma once

#include "..\Core\UIContainer.h"

class Structure;

class ReportInterface : public UIContainer
{
public:
	using TakeMeThere = NAS2D::Signals::Signal1<Structure*>;

public:
	ReportInterface() {}
	virtual ~ReportInterface() {}

public:
	/**
	 * Instructs the Report UI to clear any selections it may have.
	 */
	virtual void clearSelection() = 0;

	/**
	 * Instructs the Report UI that it should fill any lists it needs to (the
	 * panel is coming into focus).
	 */
	virtual void fillLists() = 0;

	TakeMeThere& takeMeThereCallback() { return mTakeMeThereCallback; }

private:
	TakeMeThere		mTakeMeThereCallback;
};