#ifndef __CONTROL__
#define __CONTROL__


#include "NAS2D/NAS2D.h"

using namespace NAS2D;


/**
 * \class Control
 * \brief Implements a base for all GUI Controls to derive from.
 * 
 * I know that this has already been effectively implemented in NAS2D_Gui. I rebuilt
 * it here with some cleaner design paradigms in addition to the newer event system
 * and with a stricter adherence to encapsulation principles.
 * 
 * Basically, I want this new version of the Control class to replace the version
 * that's currently part of NAS2D_Gui. The TextField and Button classes have both
 * been moved to this system. Skinning is irrelevant for the moment.
 */
class Control
{
public:

	Control();

	virtual ~Control();

	void font(Font& font);

	void position(const Point_2d& pos);
	void position(const Point_2df& pos);
	void position(float x, float y);

	float positionX();
	float positionY();

	void highlight(bool highlight);
	bool highlight() const;

	void enabled(bool enabled);
	bool enabled() const;

	void visible(bool visible);
	bool visible() const;

	void hide() { visible(false); }
	void show() { visible(true); }

	const Rectangle_2df& rect() const;

	void hasFocus(bool focus);
	bool hasFocus() const;

	void text(const std::string& text);
	const std::string& text() const;

	virtual void onMouseDown(MouseButton button, int x, int y) {};
	virtual void onMouseUp(MouseButton button, int x, int y) {};

	virtual void onKeyDown(KeyCode key, KeyModifier mod, bool repeat) {};
	virtual void onKeyUp(KeyCode key, KeyModifier mod) {};

	void size(float _s);
	void size(float w, float h);

	void width(float w);
	void height(float h);

	float width() const;
	float height() const;

	virtual void update() {};

protected:

	Font& font();
	bool fontSet() const;

	Rectangle_2df& _rect();
	std::string& _text();

	virtual void visibilityChanged(bool visible) {}

private:

	virtual void onTextChanged() {};
	virtual void onFontChanged() {};

	virtual void draw() {};

	Font*			mFont;			/**< Pointer to a Font object. Control DOES NOT own the pointer. */

	std::string		mText;			/**< Internal text string. */

	Rectangle_2df	mRect;			/**< Area of the Control. */

	bool			mEnabled;		/**< Flag indicating whether or not the Control is enabled. */
	bool			mHasFocus;		/**< Flag indicating that the Control has input focus. */
	bool			mVisible;		/**< Flag indicating visibility of the Control. */
	bool			mHighlight;		/**< Flag indicating that this Control is highlighted. */

};


#endif