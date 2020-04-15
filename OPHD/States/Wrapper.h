#pragma once

#include <NAS2D/State.h>

#include <stack>


/**
 * Implements a simple wrapper interface around the NAS2D::State object
 * to allow calls to State::update() and State::initialize() from non
 * StateManager objects.
 *
 * This class is provided as a simple means to implement a State Stack
 * without having to modify NAS2D state's and state managers.
 *
 * \note	To prevent Wrapper's and their UI's not on the top of the stack from
 *			reacting to events, Wrapper's should test if they are active in their
 *			event handler functions. If the Wrapper is returning a nullptr, it
 *			should hide its UI elements (and show them again when the reset()
 *			function is called).
 *
 */
class Wrapper : public NAS2D::State
{
public:
	Wrapper() : _active(false) {}

	State* _update() { return update(); }

	void _initialize() { initialize(); }

	void deactivate() { _active = false; _deactivate(); }
	void activate() { _active = true; _activate(); }
	bool active() { return _active; }

private:

	/**
	 * Called when the state is being deactivated.
	 *
	 * \note	This is the place to unhook event handlers.
	 */
	virtual void _deactivate() = 0;

	/**
	 * Called when the state is being reactivated and being set as the active state.
	 *
	 * \note	This function is called whenever the Wrapper is pushed onto the top
	 *			of the stack. This is the place to rehook event handlers.
	 */
	virtual void _activate() = 0;


private:
	bool	_active;
};


using WrapperStack = std::stack<Wrapper*>;
