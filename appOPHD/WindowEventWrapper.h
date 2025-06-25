#pragma once


namespace NAS2D
{
	template <typename BaseType> struct Vector;
}


/**
 * Function class used to get around the limitation in NAS2D's EventHandler
 *			Delegate system that prevents connecting stand alone functions.
 *
 * This class handles updating the configuration settings as various window
 * events occur.
 */
class WindowEventWrapper
{
public:
	WindowEventWrapper();
	~WindowEventWrapper();

private:
	void onWindowMaximized();
	void onWindowRestored();
	void onWindowResized(NAS2D::Vector<int> newSize);
};
