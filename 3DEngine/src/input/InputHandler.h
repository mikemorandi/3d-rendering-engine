#pragma once

#include <memory>

class MouseObserver;
using MouseObserverPtr = std::shared_ptr<MouseObserver>;
using MouseObserverConstPtr = std::shared_ptr<const MouseObserver>;
using MouseObserverWeakPtr = std::weak_ptr<MouseObserver>;
class KeyboardObserver;
using KeyboardObserverPtr = std::shared_ptr<KeyboardObserver>;
using KeyboardObserverConstPtr = std::shared_ptr<const KeyboardObserver>;
using KeyboardObserverWeakPtr = std::weak_ptr<KeyboardObserver>;

class InputHandler
{
public:
	virtual void AddMouseObserver(const MouseObserverWeakPtr& observer) = 0;
	virtual void AddKeyboardObserver(const KeyboardObserverWeakPtr& observer) = 0;
};