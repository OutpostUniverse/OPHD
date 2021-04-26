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
	using TextChangedCallback = NAS2D::Signal<TextControl*>;

	void text(const std::string& text);
	const std::string& text() const { return mText; }
	TextChangedCallback& textChanged() { return mTextChanged; }

	virtual void onTextChange() { mTextChanged(this); }

protected:
	TextChangedCallback mTextChanged;

	std::string mText; /**< Internal text string. */
};
