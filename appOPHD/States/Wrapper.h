#pragma once

#include <NAS2D/State.h>


/**
 * Implements a simple wrapper interface around the NAS2D::State object
 * to allow calls to State::update() and State::initialize() from non
 * StateManager objects.
 *
 * This class is provided as a simple means to provide access
 * without having to modify NAS2D states and state managers.
 *
 * \note	To prevent Wrappers and their UIs not currently active from
 *			reacting to events, Wrappers should test if they are active in their
 *			event handler functions. If the Wrapper is deactivated, it
 *			should hide its UI elements (and show them again when activated).
 */
class Wrapper : public NAS2D::State
{
public:
	Wrapper() : mIsActive(false) {}

	using NAS2D::State::initialize;
	using NAS2D::State::update;

	void deactivate() { mIsActive = false; onDeactivate(); }
	void activate() { mIsActive = true; onActivate(); }
	bool active() const { return mIsActive; }

private:

	/**
	 * Called when the state is being deactivated.
	 *
	 * \note	This is the place to unhook event handlers.
	 */
	virtual void onDeactivate() = 0;

	/**
	 * Called when the state is being reactivated and being set as the active state.
	 *
	 * \note	This is the place to rehook event handlers.
	 */
	virtual void onActivate() = 0;


private:
	bool mIsActive;
};
