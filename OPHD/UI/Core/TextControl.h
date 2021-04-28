#pragma once

#include "Control.h"

#include <NAS2D/Signal/Signal.h>

#include <string>


namespace NAS2D 
{
	class Font;
}


class TextControl : public Control
{
public:
	using TextChangeCallback = NAS2D::Signal<TextControl*>;

	void text(const std::string& text);
	const std::string& text() const { return mText; }
	TextChangeCallback::Source& textChanged() { return mTextChanged; }

	virtual void onTextChange() { mTextChanged(this); }

protected:
	TextChangeCallback mTextChanged;

	std::string mText; /**< Internal text string. */
};
