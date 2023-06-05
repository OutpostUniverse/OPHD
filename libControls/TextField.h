// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2020, Leeor Dicker
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#pragma once

#include "TextControl.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Timer.h>
#include <NAS2D/Renderer/RectangleSkin.h>


namespace NAS2D
{
	class Font;
}


/**
 * A Basic Text Field.
 *
 * The Field Control is just a set of rendered text a user can enter.
 */
class TextField : public TextControl
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

public:
	TextField();
	~TextField() override;

	void editable(bool editable);
	bool editable() const;

	bool empty() const { return text().empty(); }
	void clear() { mText.clear(); }

	void border(BorderVisibility visibility);
	void resetCursorPosition();
	void numbers_only(bool);

	void maxCharacters(std::size_t count);

	void update() override;
	void draw() const override;

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position);
	virtual void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onTextInput(const std::string& newTextInput);

private:
	void drawCursor() const;
	void updateScrollPosition();

	int textAreaWidth() const;

	const NAS2D::Font& mFont;
	const NAS2D::RectangleSkin mSkinNormal;
	const NAS2D::RectangleSkin mSkinFocus;

	NAS2D::Timer mCursorTimer; /**< Timer for the cursor blink. */

	std::size_t mCursorPosition = 0; /**< Position of the Insertion Cursor. */
	int mCursorX = 0; /**< Pixel position of the Cursor. */
	int mScrollOffset = 0; /**< Scroller offset. */

	std::size_t mMaxCharacters = 0; /**< Max number of characters allowed in the text field. */

	BorderVisibility mBorderVisibility = BorderVisibility::FocusOnly; /**< Border visibility flag. */

	bool mEditable = true; /**< Toggle editing of the field. */
	bool mShowCursor = true; /**< Flag indicating whether or not to draw the cursor. */
	bool mNumbersOnly = false; /**< Flag indicating that only numerals should be used */
};
