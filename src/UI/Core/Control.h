#pragma once

#include "NAS2D/NAS2D.h"

using namespace NAS2D;


/**
 * \class Control
 * \brief Implements a base for all GUI Controls to derive from.
 */
class Control
{
public:
	typedef NAS2D::Signals::Signal1<Control*> ResizeCallback;
	typedef NAS2D::Signals::Signal1<Control*> TextChangedCallback;
	typedef NAS2D::Signals::Signal2<float, float> PositionChangedCallback;

public:
	Control();
	virtual ~Control();

	void font(Font& font);

	void position(const Point_2d& pos);
	void position(const Point_2df& pos);
	void position(float x, float y);

	float positionX();
	float positionY();

	PositionChangedCallback& moved();

	void highlight(bool highlight);
	bool highlight() const;

	void enabled(bool enabled);
	bool enabled() const;

	void visible(bool visible);
	bool visible() const;

	virtual void hide() { visible(false); }
	virtual void show() { visible(true); }

	const Rectangle_2df& rect() const;

	virtual void hasFocus(bool focus);
	bool hasFocus() const;

	void text(const std::string& text);
	const std::string& text() const;
	TextChangedCallback& textChanged();

	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y) {};
	virtual void onMouseUp(EventHandler::MouseButton button, int x, int y) {};
	virtual void onMouseMotion(int x, int y, int dX, int dY) {};

	virtual void onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat) {};
	virtual void onKeyUp(EventHandler::KeyCode key, EventHandler::KeyModifier mod) {};

	void size(float _s);
	void size(float w, float h);

	void width(float w);
	void height(float h);

	float width() const;
	float height() const;

	ResizeCallback& resized();

	virtual void update() {};

protected:
	/**
	 * Called whenever the Control's position is changed.
	 * 
	 * \param	dX	Difference in X Position.
	 * \param	dY	Difference in Y Position.
	 */
	virtual void positionChanged(float dX, float dY) { mPositionChanged(dX, dY); }

	virtual void visibilityChanged(bool visible) {}

	virtual void enabledChanged() {};

	virtual void onFocusChanged() {};

	virtual void onSizeChanged() { mResized(this); }
	virtual void onTextChanged() { mTextChanged(this); };
	virtual void onFontChanged() {};

	Font& font();
	bool fontSet() const;

	Rectangle_2df& _rect();
	std::string& _text();

protected:
	PositionChangedCallback		mPositionChanged;	/**< Callback fired whenever the position of the Control changes. */
	ResizeCallback				mResized;
	TextChangedCallback			mTextChanged;

private:
	virtual void draw() {};

private:
	std::string		mText;				/**< Internal text string. */

	Rectangle_2df	mRect;				/**< Area of the Control. */

	Font*			mFont = nullptr;	/**< Pointer to a Font object. Control DOES NOT own the pointer. */

	bool			mEnabled = true;	/**< Flag indicating whether or not the Control is enabled. */
	bool			mHasFocus = false;	/**< Flag indicating that the Control has input focus. */
	bool			mVisible = true;	/**< Flag indicating visibility of the Control. */
	bool			mHighlight = false;	/**< Flag indicating that this Control is highlighted. */
};
