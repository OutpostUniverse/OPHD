// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2020, Leeor Dicker
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#pragma once

#include "Control.h"

#include <NAS2D/Timer.h>
#include <NAS2D/Renderer/RectangleSkin.h>
#include <NAS2D/Signal/Delegate.h>

#include <string>


namespace NAS2D
{
	enum class KeyModifier : uint16_t;
	enum class KeyCode : uint32_t;
	enum class MouseButton;

	template <typename BaseType> struct Point;

	class Font;
}


/**
 * A Basic Text Field.
 *
 * The Field Control is just a set of rendered text a user can enter.
 */
class TextField : public Control
{
public:
	/**
	* Enumerates border visibility options.
	*
	* \note	TextField defaults to a BorderVisibility::FocusOnly setting.
	*/
	enum class BorderVisibility
	{
		Never,
		Always,
		FocusOnly
	};

	using TextChangedDelegate = NAS2D::Delegate<void(TextField&)>;

public:
	TextField(std::size_t maxCharacters = 0, TextChangedDelegate textChangedHandler = {});
	~TextField() override;

	void text(const std::string& text);
	const std::string& text() const;

	bool isEmpty() const;
	void clear();

	void border(BorderVisibility visibility);
	void editable(bool editable);
	bool editable() const;
	void numbersOnly(bool);
	void maxCharacters(std::size_t count);

	void update() override;

protected:
	void updateScrollPosition();

	void draw(NAS2D::Renderer& renderer) const override;
	void drawCursor() const;

	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);
	void onTextInput(const std::string& newTextInput);
	void onTextChange();

private:
	const NAS2D::Font& mFont;
	const NAS2D::RectangleSkin mSkinNormal;
	const NAS2D::RectangleSkin mSkinFocus;
	BorderVisibility mBorderVisibility = BorderVisibility::FocusOnly;

	std::size_t mMaxCharacters = 0;
	std::string mText;
	TextChangedDelegate mTextChangedHandler;

	NAS2D::Timer mCursorBlinkTimer;
	std::size_t mCursorCharacterIndex = 0;
	int mCursorPixelX = 0;
	int mScrollOffsetPixelX = 0;

	bool mEditable = true;
	bool mShowCursor = true;
	bool mNumbersOnly = false;
};
