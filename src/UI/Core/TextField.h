// ==================================================================================
// = NAS2D+GUI
// = Copyright © 2008 - 2012, New Age Software
// ==================================================================================
// = This file is part of the NAS2D+GUI library.
// ==================================================================================

#ifndef __TEXTFIELD__
#define __TEXTFIELD__

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

	TextField();
	
	virtual ~TextField();

	void editable(bool editable);
	bool editable() const;

	bool empty() const { return text().empty(); }

	void border(BorderVisibility visibility);

	void onMouseDown(MouseButton button, int x, int y);
	void onKeyDown(KeyCode key, KeyModifier mod, bool repeat);
	void onKeyUp(KeyCode key, KeyModifier mod);

	void update();

private:

	enum ActionType
	{
		ACTION_NONE,
		ACTION_INSERT,
		ACTION_BACKSPACE,
		ACTION_DELETE,
		ACTION_MOVE_LEFT,
		ACTION_MOVE_RIGHT
	};

	void onTextChanged();
	void onFontChanged();

	void drawCursor();
	void drawTextHighlight();

	int textAreaWidth() const;

	void insertChar(KeyCode key, KeyModifier mod);
	void addCharacter(const char* character);
	
	void updateCursor();

	void draw();

	void setAction(ActionType type);
	
	Timer				mCursorTimer;		/**< Timer for the cursor blink. */
	Timer				mActionTimer;		/**< Timer used as a delay between an action and when it should start repeating. */
	Timer				mRepeatTimer;		/**< Timer used for repeating an action. */

	int 				mCursorPosition;	/**< Position of the Insertion Cursor. */
	int 				mCursorX;			/**< Pixel position of the Cursor. */
	int 				mScrollOffset;		/**< Scroller offset. */
	int					mMaxScrollOffset;	/**< Maximum allowable offset. */

	ActionType			mLastAction;		/**< Last action involving insertion, deletion and moving the cursor. */

	std::string			mInsertChar;		/**< Character to insert when repeating character insertion. */

	BorderVisibility	mBorderVisibility;	/**< Border visibility flag. */

	bool				mEditable;			/**< Toggle editing of the field. */
	bool				mShowCursor;		/**< Flag indicating whether or not to draw the cursor. */
};

#endif