#pragma once

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Resource/ResourceCache.h>


namespace NAS2D
{
	class Font;
	class Image;
}


/**
 * Implements a base for all GUI Controls to derive from.
 */
class Control
{
public:
	using ControlImageCache = NAS2D::ResourceCache<NAS2D::Image, std::string>;

	static void setDefaultFont(const NAS2D::Font& font);
	static void setDefaultFontBold(const NAS2D::Font& font);
	static void setImageCache(ControlImageCache& controlImageCache);

	static const NAS2D::Font& getDefaultFont();
	static const NAS2D::Font& getDefaultFontBold();
	static const NAS2D::Image& getImage(const std::string& filename);


	Control() = default;
	Control(Control&) = default;
	Control(Control&&) = default;
	virtual ~Control() = default;

	const NAS2D::Rectangle<int>& area() const;
	void area(const NAS2D::Rectangle<int>& area);

	NAS2D::Point<int> position() const { return mRect.position; }
	void position(NAS2D::Point<int> pos);

	NAS2D::Vector<int> size() const { return mRect.size; }
	void size(NAS2D::Vector<int> newSize);
	void size(int newSize);

	void width(int newWidth);
	void height(int newHeight);

	void highlight(bool highlight);
	bool highlight() const;

	void enabled(bool enabled);
	bool enabled() const;

	void visible(bool visible);
	bool visible() const;

	virtual void hide() { visible(false); }
	virtual void show() { visible(true); }

	virtual void hasFocus(bool focus);
	bool hasFocus() const;

	virtual void update() {}

protected:
	virtual void onMove(NAS2D::Vector<int> /*displacement*/) {}
	virtual void onResize() {}
	virtual void onVisibilityChange(bool /*visible*/) {}
	virtual void onEnableChange() {}
	virtual void onFocusChange() {}

	NAS2D::Rectangle<int> mRect; /**< Area of the Control. */

	bool mVisible = true; /**< Flag indicating visibility of the Control. */
	bool mEnabled = true; /**< Flag indicating whether or not the Control is enabled. */
	bool mHasFocus = false; /**< Flag indicating that the Control has input focus. */
	bool mHighlight = false; /**< Flag indicating that this Control is highlighted. */

private:
	virtual void draw() const {}
};
