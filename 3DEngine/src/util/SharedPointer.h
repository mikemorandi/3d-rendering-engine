#pragma once

#include <memory>

// Factory helper macro for creating shared_ptr factory methods
#define SHARED_PTR_FACTORY(T) \
	static std::shared_ptr<T> Create() \
	{ \
	return  std::make_shared<T>(); \
	}
