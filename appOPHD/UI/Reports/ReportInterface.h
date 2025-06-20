#pragma once

#include <libControls/ControlContainer.h>


class Structure;


/**
 * Provides an abstract interface for Report UIs used in the MainReportsUiState.
 *
 * \note	Inherits from ControlContainer as the report classes are treated as Control objects.
 */
class ReportInterface : public ControlContainer
{
public:
	using ControlContainer::update;

	/**
	 * Instructs the Report UI to clear any selections it may have.
	 */
	virtual void clearSelected() = 0;

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
};
