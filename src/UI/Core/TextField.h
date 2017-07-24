// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2012, New Age Software
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#pragma once

#include "NAS2D/NAS2D.h"
#include "Control.h"

using namespace NAS2D;

/**
 * \class TextField
 * \brief A Basic Text Field.
 *
 * The Field Control is just a set of rendered text a user can enter.
 */

class TextField: public Control
{
public:
	/**
	* \enum	BorderVisiblity
	* \brief	Enumerates border visibility options.
	*
	* \note	TextField defaults to a FOCUS_ONLY setting.
	*/
	enum BorderVisibility
	{
		NEVER,
		ALWAYS,
		FOCUS_ONLY
	};

public:
	TextField();
	virtual ~TextField();

	void editable(bool editable);
	bool editable() const;

	bool empty() const { return text().empty(); }

	void border(BorderVisibility visibility);
	void resetCursorPosition();
	void numbers_only(bool);

	void maxCharacters(size_t count);

	void update();

protected:
	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y);
	virtual void onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat);
	void onTextInput(const std::string&);

private:
	void onFontChanged();

	void drawCursor();
	void drawTextHighlight();

	int textAreaWidth() const;

	void updateCursor();

	void draw();

private:
	Timer				mCursorTimer;		/**< Timer for the cursor blink. */

	int 				mCursorPosition;	/**< Position of the Insertion Cursor. */
	int 				mCursorX;			/**< Pixel position of the Cursor. */
	int 				mScrollOffset;		/**< Scroller offset. */
	int					mMaxScrollOffset;	/**< Maximum allowable offset. */

	size_t				mMaxCharacters;		/**< Max number of characters allowed in the text field. */

	BorderVisibility	mBorderVisibility;	/**< Border visibility flag. */

	ImageList			mSkinNormal;
	ImageList			mSkinFocus;

	bool				mEditable;			/**< Toggle editing of the field. */
	bool				mShowCursor;		/**< Flag indicating whether or not to draw the cursor. */
	bool				mNumbersOnly;		/**< Flag indicating that only numerals should be used */
};
