#pragma once

#include "Control.h"

#include <NAS2D/Signal/Signal.h>

#include <string>


class TextControl : public Control
{
public:
	using TextChangeSignal = NAS2D::Signal<TextControl*>;

	void text(const std::string& text);
	const std::string& text() const { return mText; }
	TextChangeSignal::Source& textChanged() { return mTextChanged; }

	virtual void onTextChange() { mTextChanged(this); }

protected:
	TextChangeSignal mTextChanged;

	std::string mText; /**< Internal text string. */
};
