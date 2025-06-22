#pragma once

#include <libControls/ControlContainer.h>


class Structure;


/**
 * Provides an abstract interface for Report used in the ReportsState.
 *
 * \note	Inherits from ControlContainer as the report classes are treated as Control objects.
 */
class Report : public ControlContainer
{
public:
	/**
	 * Instructs a Report to set its primary selection to a specified Structure.
	 *
	 * \note	Casting may be necessary depending on implementation of inheriting
	 *			classes. Be mindful to pass pointers to objects that can be safely
	 *			downcasted to a more derived type (take advantage of dynamic_cast)
	 */
	virtual void selectStructure(Structure*) = 0;

	/**
	 * Instructs the Report to clear any selections it may have.
	 */
	virtual void clearSelected() = 0;

	/**
	 * Instructs the Report that it should fill any lists it needs to (the
	 * panel is coming into focus).
	 */
	virtual void fillLists() = 0;

	/**
	 * Instructs the Report to refresh itself. Used to clear any state
	 * information to a 'like new' state.
	 */
	virtual void refresh() = 0;

	using ControlContainer::update;
};
