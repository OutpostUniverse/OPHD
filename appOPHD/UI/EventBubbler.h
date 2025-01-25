#pragma once

#include <NAS2D/EventHandler.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <memory>

class EventBubbler
{
public:
	using KeyCode = NAS2D::EventHandler::KeyCode;
	using KeyModifier = NAS2D::EventHandler::KeyModifier;
	using MouseButton = NAS2D::EventHandler::MouseButton;

	using KeyDownHandler = std::function<bool(KeyCode, KeyModifier, bool)>;
	using KeyUpHandler = std::function<bool(KeyCode, KeyModifier)>;
	using MouseDownHandler = std::function<bool(MouseButton, NAS2D::Point<int>)>;
	using MouseUpHandler = std::function<bool(MouseButton, NAS2D::Point<int>)>;
	using MouseDoubleClickHandler = std::function<bool(MouseButton, NAS2D::Point<int>)>;
	using MouseMoveHandler = std::function<bool(NAS2D::Point<int>, NAS2D::Vector<int>)>;
	using MouseWheelHandler = std::function<bool(NAS2D::Vector<int>)>;

	void keyDownHandler(KeyDownHandler handler) { mKeyDownHandler = handler; }
	void keyUpHandler(KeyUpHandler handler) { mKeyUpHandler = handler; }
	void mouseDownHandler(MouseDownHandler handler) { mMouseDownHandler = handler; }
	void mouseUpHandler(MouseUpHandler handler) { mMouseUpHandler = handler; }
	void mouseDoubleClickHandler(MouseDoubleClickHandler handler) { mMouseDoubleClickHandler = handler; }
	void mouseMoveHandler(MouseMoveHandler handler) { mMouseMoveHandler = handler; }
	void mouseWheelHandler(MouseWheelHandler handler) { mMouseWheelHandler = handler; }


	bool onKeyDown(KeyCode key, KeyModifier mod, bool repeat) { return mKeyDownHandler ? mKeyDownHandler(key, mod, repeat) : false; }
	bool onKeyUp(KeyCode key, KeyModifier mod) {return mKeyUpHandler ? mKeyUpHandler(key, mod) : false; }
	bool onMouseDown(MouseButton button, NAS2D::Point<int> position) {return mMouseDownHandler ? mMouseDownHandler(button, position) : false; }
	bool onMouseUp(MouseButton button, NAS2D::Point<int> position) {return mMouseUpHandler ? mMouseUpHandler(button, position) : false; }
	bool onMouseDoubleClick(MouseButton button, NAS2D::Point<int> position) {return mMouseDoubleClickHandler ? mMouseDoubleClickHandler(button, position) : false; }
	bool onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative) {return mMouseMoveHandler ? mMouseMoveHandler(position, relative) : false; }
	bool onMouseWheel(NAS2D::Vector<int> delta) {return mMouseWheelHandler ? mMouseWheelHandler(delta) : false; }


	void addChild(std::shared_ptr<EventBubbler> child)
	{
		mChildren.push_back(child);
	}


	void removeChild(EventBubbler* child)
	{
	mChildren.erase(std::remove_if(mChildren.begin(), mChildren.end(), 
		[child](const std::weak_ptr<EventBubbler>& weakPtr) {
			if (auto sharedPtr = weakPtr.lock())
			{
				return sharedPtr.get() == child;
			}
			return false;
		}),
		mChildren.end());
	}


	std::shared_ptr<EventBubbler> children(size_t index) const
	{
		if (index < mChildren.size())
		{
			auto it = mChildren.begin();
			std::advance(it, index);
			auto sharedPtr = it->lock();
			if (sharedPtr)
			{
				return sharedPtr;
			}
		}
		return nullptr;
	}


protected:
	KeyDownHandler mKeyDownHandler;
	KeyUpHandler mKeyUpHandler;
	MouseDownHandler mMouseDownHandler;
	MouseUpHandler mMouseUpHandler;
	MouseDoubleClickHandler mMouseDoubleClickHandler;
	MouseMoveHandler mMouseMoveHandler;
	MouseWheelHandler mMouseWheelHandler;

	std::vector<std::weak_ptr<EventBubbler>> mChildren;
};
