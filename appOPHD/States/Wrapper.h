#pragma once

#include <NAS2D/State.h>

#include <stack>


/**
 * Implements a simple wrapper interface around the NAS2D::State object
 * to allow calls to State::update() and State::initialize() from non
 * StateManager objects.
 *
 * This class is provided as a simple means to implement a State Stack
 * without having to modify NAS2D states and state managers.
 *
 * \note	To prevent Wrappers and their UIs not on the top of the stack from
 *			reacting to events, Wrappers should test if they are active in their
 *			event handler functions. If the Wrapper is returning a nullptr, it
 *			should hide its UI elements (and show them again when the reset()
 *			function is called).
 *
 */
class Wrapper : public NAS2D::State
{
public:
	Wrapper() : mIsActive(false) {}

	using NAS2D::State::initialize;
	using NAS2D::State::update;

	void deactivate() { mIsActive = false; onDeactivate(); }
	void activate() { mIsActive = true; _activate(); }
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
	 * \note	This function is called whenever the Wrapper is pushed onto the top
	 *			of the stack. This is the place to rehook event handlers.
	 */
	virtual void _activate() = 0;


private:
	bool mIsActive;
};


using WrapperStack = std::stack<Wrapper*>;
