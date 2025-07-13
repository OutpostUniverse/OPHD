#pragma once

#include "Control.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Math/Point.h>

#include <string>


namespace NAS2D
{
	enum class MouseButton;
	class Font;
	class Image;
}


class CheckBox : public Control
{
public:
	using ClickDelegate = NAS2D::Delegate<void()>;

	CheckBox(std::string newText = {}, ClickDelegate clickHandler = {});
	~CheckBox() override;

	void checked(bool toggle);
	bool checked() const;

protected:
	void draw(NAS2D::Renderer& renderer) const override;

	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);

	void onResize() override;
	void onTextChange();

private:
	const NAS2D::Font& mFont;
	const NAS2D::Image& mSkin;
	const std::string mText;
	ClickDelegate mClickHandler;
	bool mChecked = false;
};
