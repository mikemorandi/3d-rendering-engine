#pragma once

#include "../util/SharedPointer.h"

SHARED_PTR_CLASS_DECL(MouseObserver);
SHARED_PTR_CLASS_DECL(KeyboardObserver);

class InputHandler
{
public:
	virtual void AddMouseObserver(const MouseObserver_wptr& observer) = 0;
	virtual void AddKeyboardObserver(const KeyboardObserver_wptr& observer) = 0;
};