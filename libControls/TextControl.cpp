#include "TextControl.h"

#include <NAS2D/Resource/Font.h> 

/**
 * Sets the text of the Control.
 */
void TextControl::text(const std::string& text)
{
	mText = text;
	onTextChange();
}


void TextControl::onTextChange()
{
	if (mFont)
	{
		mTextSize = mFont->size(mText);
	}
	mTextChanged(this);
}
