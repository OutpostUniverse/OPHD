#pragma once

#include "Control.h"

#include <string>


class TextControl : public Control
{
public:
	void text(const std::string& text);
	const std::string& text() const { return mText; }

	virtual void onTextChange() {}

protected:
	std::string mText;
};
