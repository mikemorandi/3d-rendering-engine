#pragma once

namespace GLErrorHandler {
	/// Initialize OpenGL error handling that terminates on any GL error
	/// Prints detailed error information including function name, error code, and stack trace
	void Initialize();
}
