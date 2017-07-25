#pragma once

#include "NAS2D/NAS2D.h"

#include "Control.h"

#include <string>

using namespace NAS2D;

class ProgressBar : public Control
{
public:

	enum ImageMode
	{
		Stretching,
		Repeating,
		Straight
	};

public:
	typedef NAS2D::Signals::Signal0<void> ClickCallback;

public:
	ProgressBar();
	ProgressBar(size_t end);
	~ProgressBar();

	void setColor(Uint8 _r, Uint8 _g, Uint8 _b) { setColor(_r, _g, _b,255); }
	void setColor(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a) { mColorR = _r; mColorG = _g; mColorB = _b; mColorAlpha = _a; }
	void image(const std::string& _path) { image(_path, ImageMode::Repeating); }
	void image(const std::string& _path, ImageMode _m);
	bool hasImage() const;
	
	void End(size_t _End) { mEnd = _End; }
	size_t End() { return mEnd; }
	void Cursor(size_t _Cursor) { mCurrent = _Cursor; }
	size_t Cursor() { return mCurrent; }

	void update();

protected:
	size_t mCurrent;
	size_t mEnd;

private:
	void draw();

private:
	Uint8			mColorR;
	Uint8			mColorG;
	Uint8			mColorB;
	Uint8			mColorAlpha;

	Image			mImage;
	ImageMode		mImageMode;

	ImageList		mSkinOut;
	ImageList		mSkinIn;

	bool			mUsesImage;			/**< Internal flag indicating that the Button uses an image graphic. */
};
