#ifndef __BUTTON__
#define __BUTTON__

#include "NAS2D/NAS2D.h"

#include "Control.h"

#include <string>

using namespace NAS2D;

class Button: public Control
{
public:

	typedef Gallant::Signal0<void> ClickCallback;

	enum Type
	{
		BUTTON_NORMAL,
		BUTTON_TOGGLE
	};

	Button();
	~Button();

	void type(Type type);

	void toggle(bool toggle);
	bool toggled() const;

	void image(const std::string& path);
	bool hasImage() const;
	
	void onMouseDown(MouseButton button, int x, int y);
	void onMouseUp(MouseButton button, int x, int y);

	ClickCallback& click() { return mCallback; }

	void update();

private:

	enum State
	{
		STATE_NORMAL,
		STATE_PRESSED
	};

	void draw();
	void drawUp();
	void drawDown();


	State			mState;				/**< Current state of the button. */
	Type			mType;				/**< Modifies button behavior. */

	Image			mImage;				/**< Image to draw centered on the button. */

	ClickCallback	mCallback;			/**< Object to notify when the button is activated. */

	bool			mUsesImage;			/**< Internal flag indicating that this button uses an image graphic. */
};

#endif