# graphics-playground Executable
add_executable(graphics-playground
    "graphics-playground.cpp"
    "graphics-playground.h"
    "GlfwInputHandler.cpp"
    "GlfwInputHandler.h"
)

# Set target properties
set_target_properties(graphics-playground PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories
target_include_directories(graphics-playground PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(graphics-playground PRIVATE
    3DEngine
    glfw
    glbinding::glbinding
    glbinding::glbinding-aux
    glm::glm
    ${PLATFORM_LIBS}
)

# Compiler warnings
if(MSVC)
    target_compile_options(graphics-playground PRIVATE /W3)
    # Disable specific MSVC warnings for glbinding
    target_compile_options(graphics-playground PRIVATE /wd4251)
else()
    target_compile_options(graphics-playground PRIVATE -Wall -Wextra -Wpedantic)
    # Suppress deprecation warnings for OpenGL on macOS
    if(APPLE)
        target_compile_options(graphics-playground PRIVATE -Wno-deprecated-declarations)
    endif()
endif()

# Platform-specific definitions
if(WIN32)
    target_compile_definitions(graphics-playground PRIVATE WIN32 _CONSOLE)
elseif(APPLE)
    # Required for macOS OpenGL compatibility
    target_compile_definitions(graphics-playground PRIVATE GL_SILENCE_DEPRECATION)
endif()

# Set working directory to project root for easier data access
# This is mainly for IDEs like Visual Studio, Xcode, etc.
set_target_properties(graphics-playground PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Install rules
install(TARGETS graphics-playground
    RUNTIME DESTINATION bin
)

# Copy data directory to build directory for easier testing
# This creates a symlink on Unix or copies on Windows
if(CMAKE_HOST_UNIX)
    add_custom_command(TARGET graphics-playground POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_SOURCE_DIR}/data
            ${CMAKE_BINARY_DIR}/bin/data
        COMMENT "Creating symlink to data directory"
    )
else()
    add_custom_command(TARGET graphics-playground POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            ${CMAKE_BINARY_DIR}/bin/data
        COMMENT "Copying data directory"
    )
endif()
