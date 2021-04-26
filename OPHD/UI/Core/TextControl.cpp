#include "TextControl.h"

/**
 * Sets the text of the Control.
 */
void TextControl::text(const std::string& text)
{
	mText = text;
	onTextChange();
}
