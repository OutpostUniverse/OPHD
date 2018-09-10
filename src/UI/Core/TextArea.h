#pragma once

#include "NAS2D/NAS2D.h"

#include "Control.h"

#include <string>

using namespace NAS2D;

class TextArea : public Control
{
public:
	TextArea();
	virtual ~TextArea();

	void textColor(int r, int g, int b, int a = 255) { mTextColor(r, g, b, a); }

	void update();

private:
	typedef std::vector<std::string> StringList;

private:
	virtual void onSizeChanged();
	virtual void onTextChanged();
	virtual void onFontChanged();

	virtual void draw();
	void processString();

private:
	StringList	mFormattedList;

	Color_4ub	mTextColor = COLOR_WHITE;

	size_t		mNumLines = 0;
};
