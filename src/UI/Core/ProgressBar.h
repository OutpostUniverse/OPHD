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

	void setColor(unsigned char _r, unsigned char _g, unsigned char _b) { setColor(_r, _g, _b,255); }
	void setColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) { mColorR = _r; mColorG = _g; mColorB = _b; mColorAlpha = _a; }
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
	void init();
	void draw();

private:
	unsigned char			mColorR;
	unsigned char			mColorG;
	unsigned char			mColorB;
	unsigned char			mColorAlpha;

	Image			mImage;
	ImageMode		mImageMode;

	ImageList		mSkinOut;
	ImageList		mSkinIn;

	bool			mUsesImage;			/**< Internal flag indicating that the Button uses an image graphic. */
};
