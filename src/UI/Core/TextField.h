// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2012, New Age Software
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#pragma once

#include "NAS2D/NAS2D.h"
#include "Control.h"


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
	void clear() { _text().clear(); }

	void border(BorderVisibility visibility);
	void resetCursorPosition();
	void numbers_only(bool);

	void maxCharacters(size_t count);

	void update();

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onTextInput(const std::string&);

private:
	void drawCursor();
	void drawTextHighlight();

	int textAreaWidth() const;

	void updateCursor();

private:
	NAS2D::Timer		mCursorTimer;					/**< Timer for the cursor blink. */

	int 				mCursorPosition = 0;			/**< Position of the Insertion Cursor. */
	int 				mCursorX = 0;					/**< Pixel position of the Cursor. */
	int 				mScrollOffset = 0;				/**< Scroller offset. */
	int					mMaxScrollOffset = 0;			/**< Maximum allowable offset. */

	size_t				mMaxCharacters = 0;				/**< Max number of characters allowed in the text field. */

	BorderVisibility	mBorderVisibility = FOCUS_ONLY;	/**< Border visibility flag. */

	NAS2D::ImageList	mSkinNormal;
	NAS2D::ImageList	mSkinFocus;

	bool				mEditable = true;				/**< Toggle editing of the field. */
	bool				mShowCursor = true;				/**< Flag indicating whether or not to draw the cursor. */
	bool				mNumbersOnly = false;			/**< Flag indicating that only numerals should be used */
};
