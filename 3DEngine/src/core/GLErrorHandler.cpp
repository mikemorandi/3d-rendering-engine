#include "GLErrorHandler.h"
#include "gl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <execinfo.h>
#include <cxxabi.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#include <glbinding/FunctionCall.h>
#include <glbinding/CallbackMask.h>
#include <glbinding/AbstractFunction.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

using namespace gl;

namespace GLErrorHandler {

static std::ofstream g_TraceFile;
static bool g_TracingEnabled = false;
static int g_CallCount = 0;

void EnableTracing(const std::string& filename) {
	g_TraceFile.open(filename);
	g_TracingEnabled = g_TraceFile.is_open();
	if (g_TracingEnabled) {
		std::cout << "[GLTrace] Tracing to: " << filename << std::endl;
	}
}

void Initialize() {
	// Tracing disabled by default - enable with EnableTracing() if needed for debugging
	// EnableTracing("gl_trace.log");
	
	// Set up error callback that terminates on OpenGL errors
	glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, { "glGetError" });
	glbinding::setAfterCallback([](const glbinding::FunctionCall& call) {
		const auto error = glGetError();
		if (error != GL_NO_ERROR) {
			// Convert error to integer for printing
			unsigned int errorCode = static_cast<unsigned int>(error);

			std::cerr << "\n=== OpenGL Error ===" << std::endl;
			std::cerr << "Function: " << call.function->name() << std::endl;
			std::cerr << "Error Code: 0x" << std::hex << errorCode << std::dec;

			// Print human-readable error name
			switch(errorCode) {
				case 0x0500: std::cerr << " (GL_INVALID_ENUM)"; break;
				case 0x0501: std::cerr << " (GL_INVALID_VALUE)"; break;
				case 0x0502: std::cerr << " (GL_INVALID_OPERATION)"; break;
				case 0x0503: std::cerr << " (GL_STACK_OVERFLOW)"; break;
				case 0x0504: std::cerr << " (GL_STACK_UNDERFLOW)"; break;
				case 0x0505: std::cerr << " (GL_OUT_OF_MEMORY)"; break;
				case 0x0506: std::cerr << " (GL_INVALID_FRAMEBUFFER_OPERATION)"; break;
			}
			std::cerr << std::endl;

			// Print stack trace
			std::cerr << "\nStack trace:" << std::endl;
			void* callstack[128];
			int frames = backtrace(callstack, 128);
			char** symbols = backtrace_symbols(callstack, frames);

			for (int i = 0; i < frames; i++) {
				std::string symbol(symbols[i]);

				// Try to demangle C++ symbols
				size_t begin = symbol.find('_');
				size_t end = symbol.find(" + ", begin);
				if (begin != std::string::npos && end != std::string::npos) {
					std::string mangled = symbol.substr(begin, end - begin);
					int status;
					char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
					if (status == 0) {
						symbol.replace(begin, end - begin, demangled);
						free(demangled);
					}
				}

				std::cerr << "  " << i << ": " << symbol << std::endl;
			}
			free(symbols);

			std::cerr << "\nTerminating due to OpenGL error." << std::endl;
			
			// Flush trace file before exit
			if (g_TracingEnabled && g_TraceFile.is_open()) {
				g_TraceFile << "=== ERROR at call " << g_CallCount << " ===" << std::endl;
				g_TraceFile.flush();
				g_TraceFile.close();
			}
			
			std::exit(1);
		}
	});
}

} // namespace GLErrorHandler
