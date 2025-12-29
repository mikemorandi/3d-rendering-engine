# GoogleTest setup
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Test executable
add_executable(3DEngine_tests
    BoundingBoxTest.cpp
)

# Link against gtest, gtest_main, and the 3DEngine library
target_link_libraries(3DEngine_tests
    PRIVATE
    gtest
    gtest_main
    3DEngine
)

# Include directories
target_include_directories(3DEngine_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(3DEngine_tests)
