#pragma once

#include "../Core/UIContainer.h"

class Structure;

/**
 * Provides an abstract interface for Report UI's used in the MainReportsUiState.
 * 
 * \note	Inherits from UIContainer as the report classes are treated as UI objects.
 */
class ReportInterface : public UIContainer
{
public:
	/**
	 * Callback signal used to handle clicks of a "Take Me There" button to center
	 * the map view on a given structure.
	 */
	using TakeMeThere = NAS2D::Signals::Signal<Structure*>;

public:
	ReportInterface() {}

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

	/**
	 * Instructs the Report UI to refresh itself. Used to clear any state
	 * information to a 'like new' state.
	 */
	virtual void refresh() = 0;

	/**
	 * Instructs a Report UI to set its primary selection to a specified Structure.
	 * 
	 * \note	Casting may be necessary depending on implementation of inheriting
	 *			classes. Be mindful to pass pointers to objects that can be safely
	 *			downcasted to a more derived type (take advantage of dynamic_cast)
	 */
	virtual void selectStructure(Structure*) = 0;

	TakeMeThere& takeMeThereCallback() { return mTakeMeThereCallback; }

private:
	TakeMeThere		mTakeMeThereCallback;
};
