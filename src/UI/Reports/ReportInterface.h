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
	virtual void clearSelection() = 0;
	virtual void fillLists() = 0;

	TakeMeThere& takeMeThereCallback() { return mTakeMeThereCallback; }

private:
	TakeMeThere		mTakeMeThereCallback;
};