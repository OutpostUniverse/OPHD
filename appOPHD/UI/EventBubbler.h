#pragma once

#include <vector>

class EventBubbler
{
public:
	virtual ~EventBubbler() = default;
	virtual bool handleEvent() = 0;

private:	
	virtual void addChild(EventBubbler&) = 0;
};
