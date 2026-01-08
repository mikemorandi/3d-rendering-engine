#pragma once

#include <glm/fwd.hpp>

namespace Input
{
	enum class Key {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,ARROW_UP,ARROW_DOWN,ARROW_LEFT,ARROW_RIGHT,SPACE,ESCAPE};
	enum class Modifier { NONE, SHIFT, CONTROL, ALT };
	enum class KeyState { PRESSED, RELEASED };
};

class KeyboardObserver
{
public:

	virtual void OnKey(const Input::Key key, const Input::Modifier mod) = 0;
	virtual void OnKeyStateChange([[maybe_unused]] const Input::Key key, [[maybe_unused]] const Input::KeyState state) { }
};