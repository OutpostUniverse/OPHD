#include "MessageBox.h"

#if defined(WINDOWS) || defined(WIN32)
	#include <Windows.h>
	#include <SDL2/SDL_syswm.h>
#else
	#include <SDL2/SDL.h>
#endif


#if defined(WINDOWS) || defined(WIN32)
/**
 * Gets a Windows API HWND handle to the main
 * window of the application.
 * 
 * \note	This is only exposed for Windows applications.
 * 
 * \warning	Uses internal knowledge of NAS2D to force an
 *			export that should not be exposed. This will
 *			break if NAS2D changes its structure.
 */
HWND WIN32_getWindowHandle()
{
	SDL_SysWMinfo systemInfo;
	SDL_VERSION(&systemInfo.version);

	/** \fixme Evil hack exposing an internal NAS2D variable. */
	extern SDL_Window* underlyingWindow;
	if (SDL_GetWindowWMInfo(underlyingWindow, &systemInfo) != 1)
	{
		return nullptr;
	}

	return systemInfo.info.win.window;
}
#endif


/**
 * Shows a message dialog box.
 */
void doNonFatalErrorMessage(const std::string& title, const std::string& msg)
{
#if defined(WINDOWS) || defined(WIN32)
	MessageBoxA(WIN32_getWindowHandle(), msg.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), msg.c_str(), NULL);
#endif
}


/**
 * Shows a message dialog box with no icon.
 */
void doAlertMessage(const std::string& title, const std::string& msg)
{
#if defined(WINDOWS) || defined(WIN32)
	MessageBoxA(WIN32_getWindowHandle(), msg.c_str(), title.c_str(), MB_OK | /*MB_ICONINFORMATION |*/ MB_TASKMODAL);
#else
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, title.c_str(), msg.c_str(), NULL);
#endif
}


/**
 * Shows a message dialog box with Yes and No buttons.
 */
bool doYesNoMessage(const std::string& title, const std::string msg)
{
	bool yes = false;
#if defined(WINDOWS) || defined(WIN32)
	yes = (MessageBoxA(WIN32_getWindowHandle(), msg.c_str(), title.c_str(), MB_YESNO | MB_ICONINFORMATION | MB_TASKMODAL) == IDYES);
#else
	const SDL_MessageBoxButtonData buttons[] =
	{
		{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No"},
		{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes"}
	};

	const SDL_MessageBoxData messageboxdata =
	{
		SDL_MESSAGEBOX_INFORMATION,
		nullptr,
		title.c_str(),
		msg.c_str(),
		SDL_arraysize(buttons),
		buttons,
		nullptr
	};

	int buttonId = 0;
	SDL_ShowMessageBox(&messageboxdata, &buttonId);
	yes = (buttonId == 1);
#endif

	return yes;
}


/**
 * Bit of a kludge to get state of a window
 * outside of what the EventHandler provides.
 */
bool windowMaximized()
{
	extern SDL_Window* underlyingWindow;
	unsigned int flags = SDL_GetWindowFlags(underlyingWindow);
	return (flags & SDL_WINDOW_MAXIMIZED);
}
