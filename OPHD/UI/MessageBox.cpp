#include "MessageBox.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


/**
 * Shows a message dialog box.
 */
void doNonFatalErrorMessage(const std::string& title, const std::string& message)
{
	NAS2D::Utility<NAS2D::Renderer>::get().doModalError(title, message);
}


/**
 * Shows a message dialog box with no icon.
 */
void doAlertMessage(const std::string& title, const std::string& message)
{
	NAS2D::Utility<NAS2D::Renderer>::get().doModalAlert(title, message);
}


/**
 * Shows a message dialog box with Yes and No buttons.
 */
bool doYesNoMessage(const std::string& title, const std::string message)
{
	return NAS2D::Utility<NAS2D::Renderer>::get().doModalYesNo(title, message);
}
