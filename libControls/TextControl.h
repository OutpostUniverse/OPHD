#pragma once

#include "Control.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Math/Vector.h>

#include <string>


class TextControl : public Control
{
public:
	using TextChangeSignal = NAS2D::Signal<TextControl*>;

	void text(const std::string& text);
	const std::string& text() const { return mText; }
	TextChangeSignal::Source& textChanged() { return mTextChanged; }
	NAS2D::Vector<int> minimumSize() const { return mTextSize; }

	virtual void onTextChange();

protected:
	TextChangeSignal mTextChanged;
	const NAS2D::Font* mFont = nullptr;
	NAS2D::Vector<int> mTextSize;

	std::string mText; /**< Internal text string. */
};
