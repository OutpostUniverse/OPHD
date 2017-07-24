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

	typedef NAS2D::Signals::Signal0<void> ClickCallback;

	ProgressBar();
	ProgressBar(Uint32 _End);
	~ProgressBar();

	void setColor(Uint8 _r, Uint8 _g, Uint8 _b) { setColor(_r, _g, _b,255); }
	void setColor(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a) { mColorR = _r; mColorG = _g; mColorB = _b; mColorAlpha = _a; }
	void image(const std::string& _path) { image(_path, ImageMode::Repeating); }
	void image(const std::string& _path, ImageMode _m);
	bool hasImage() const;
	

	void update();

	void End(Uint32 _End) { mEnd = _End; }
	Uint32 End() { return mEnd; }
	void Cursor(Uint32 _Cursor) { mCurrent = _Cursor; }
	Uint32 Cursor() { return mCurrent; }
protected:
	Uint32 mCurrent;
	Uint32 mEnd;
private:

	void draw();

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
