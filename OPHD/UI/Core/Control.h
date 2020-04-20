#pragma once

#include "NAS2D/Signal.h"
#include "NAS2D/Renderer/Point.h"
#include "NAS2D/Renderer/Rectangle.h"

#include <string>

/**
 * \class Control
 * \brief Implements a base for all GUI Controls to derive from.
 * 
 * The Control class is the base class from which all GUI controls inherit
 * from.
 */
class Control
{
public:
	using ResizeCallback = NAS2D::Signals::Signal<Control*>;
	using TextChangedCallback = NAS2D::Signals::Signal<Control*>;
	using PositionChangedCallback = NAS2D::Signals::Signal<float, float>;

public:
	Control() = default;
	virtual ~Control() = default;

	NAS2D::Point<float> position() const { return mRect.startPoint(); }
	void position(const NAS2D::Point<float>& pos);
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

	const NAS2D::Rectangle<float>& rect() const;

	virtual void hasFocus(bool focus);
	bool hasFocus() const;

	void text(const std::string& text);
	const std::string& text() const { return mText; }
	TextChangedCallback& textChanged() { return mTextChanged; }

	NAS2D::Vector<float> size() const { return mRect.size(); }
	void size(NAS2D::Vector<float> newSize);
	void size(float newSize);

	void width(float w);
	void height(float h);

	float width() const;
	float height() const;

	ResizeCallback& resized();

	virtual void update() {}

protected:
	/**
	 * Called whenever the Control's position is changed.
	 * 
	 * \param	dX	Difference in X Position.
	 * \param	dY	Difference in Y Position.
	 */
	virtual void positionChanged(float dX, float dY) { mPositionChanged(dX, dY); }

	virtual void visibilityChanged(bool /*visible*/) {}

	virtual void enabledChanged() {}

	virtual void onFocusChanged() {}

	virtual void onSizeChanged() { mResized(this); }
	virtual void onTextChanged() { mTextChanged(this); }

protected:
	PositionChangedCallback mPositionChanged; /**< Callback fired whenever the position of the Control changes. */
	ResizeCallback mResized;
	TextChangedCallback mTextChanged;

	NAS2D::Rectangle<float> mRect; /**< Area of the Control. */

private:
	virtual void draw() {}

protected:
	std::string mText; /**< Internal text string. */

	bool mEnabled = true; /**< Flag indicating whether or not the Control is enabled. */
	bool mHasFocus = false; /**< Flag indicating that the Control has input focus. */
	bool mVisible = true; /**< Flag indicating visibility of the Control. */
	bool mHighlight = false; /**< Flag indicating that this Control is highlighted. */
};
