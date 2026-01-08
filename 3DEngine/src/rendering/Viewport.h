#pragma once

#include <memory>

class Viewport;
using ViewportPtr = std::shared_ptr<Viewport>;
using ViewportConstPtr = std::shared_ptr<const Viewport>;
using ViewportWeakPtr = std::weak_ptr<Viewport>;

class Viewport
{
public:

	static ViewportPtr Create(int width, int height);

	Viewport(int width, int height);

	void Apply();

	virtual ~Viewport();

public:

	int width;
	int height;

};