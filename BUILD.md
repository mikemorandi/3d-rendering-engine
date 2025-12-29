# Build Instructions

This document describes how to build the 3D Rendering Engine on different platforms.

## Prerequisites

### All Platforms

- **CMake** 3.20 or higher ([download](https://cmake.org/download/))
- **C++17 compatible compiler**
  - Windows: Visual Studio 2019 or newer, or MSVC v142+
  - macOS: Clang 10+ (Xcode Command Line Tools)
  - Linux: GCC 9+ or Clang 10+
- **vcpkg** for dependency management ([installation guide](https://github.com/Microsoft/vcpkg))

### Platform-Specific Requirements

#### macOS
- Xcode Command Line Tools: `xcode-select --install`
- OpenGL 4.1 (built into macOS, but deprecated)

#### Linux
- OpenGL development libraries: `sudo apt-get install libgl1-mesa-dev` (Ubuntu/Debian)

#### Windows
- Visual Studio 2019 or newer with C++ desktop development workload

---

## Installing Dependencies

### Option 1: Using vcpkg (Recommended)

1. **Install vcpkg** if you haven't already:
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh  # macOS/Linux
   # OR
   .\bootstrap-vcpkg.bat  # Windows
   ```

2. **Install project dependencies:**

   **macOS:**
   ```bash
   ./vcpkg install glfw3:x64-osx glbinding:x64-osx glm:x64-osx tinyxml2:x64-osx
   ```

   **Linux:**
   ```bash
   ./vcpkg install glfw3:x64-linux glbinding:x64-linux glm:x64-linux tinyxml2:x64-linux
   ```

   **Windows:**
   ```bash
   .\vcpkg install glfw3:x64-windows glbinding:x64-windows glm:x64-windows tinyxml2:x64-windows
   ```

3. **Note the vcpkg toolchain file path** (you'll need it for CMake):
   ```
   [path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
   ```

---

## Building the Project

### macOS

```bash
# Navigate to project directory
cd 3d-rendering-engine

# Create build directory
mkdir build && cd build

# Configure with CMake (replace [vcpkg-root] with your vcpkg path)
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake \
         -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# The executable will be in: build/bin/Graphics Playground
```

**Optional: Generate Xcode project**
```bash
cmake .. -G Xcode -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
open 3DRenderingEngine.xcodeproj
```

### Linux

```bash
# Navigate to project directory
cd 3d-rendering-engine

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake \
         -DCMAKE_BUILD_TYPE=Release

# Build (use -j for parallel build)
cmake --build . --config Release -j$(nproc)

# The executable will be in: build/bin/Graphics Playground
```

### Windows

**Using Command Line:**
```cmd
REM Navigate to project directory
cd 3d-rendering-engine

REM Create build directory
mkdir build
cd build

REM Configure with CMake (Visual Studio 2019)
cmake .. -G "Visual Studio 16 2019" -A x64 ^
         -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake

REM Build
cmake --build . --config Release

REM The executable will be in: build\bin\Release\Graphics Playground.exe
```

**Using Visual Studio:**
1. Open Visual Studio 2019+
2. File → Open → CMake... → Select `CMakeLists.txt`
3. Configure vcpkg toolchain in CMakeSettings.json:
   ```json
   {
     "configurations": [{
       "name": "x64-Release",
       "generator": "Visual Studio 16 2019",
       "configurationType": "Release",
       "buildRoot": "${projectDir}\\build",
       "cmakeToolchain": "[vcpkg-root]/scripts/buildsystems/vcpkg.cmake"
     }]
   }
   ```
4. Build → Build All

---

## Running the Application

### Setting Working Directory

The application needs to be run with the **project root** as the working directory to access data files (shaders, models, textures).

**macOS/Linux:**
```bash
# From project root
./build/bin/"Graphics Playground" [scene.xml]

# Example with a specific scene
./build/bin/"Graphics Playground" road.xml
```

**Windows:**
```cmd
REM From project root
build\bin\Release\"Graphics Playground.exe" [scene.xml]

REM Example
build\bin\Release\"Graphics Playground.exe" road.xml
```

### Available Scenes

The application will list available scenes if no argument is provided:
- `road.xml` - Car on road with street lamps
- `blackhawk.xml` - Blackhawk helicopter
- `spotlights.xml` - Spotlight demonstration
- `shScene.xml` - Spherical Harmonics lighting
- `headScene.xml` - Head model
---

## Troubleshooting

### macOS: OpenGL Deprecation Warnings

**Expected behavior:** You will see deprecation warnings when running on macOS 10.14+:
```
OpenGL is deprecated. Consider using Metal instead.
```

This is normal. OpenGL 4.1 still works on macOS but is no longer maintained by Apple. For a future-proof solution, consider migrating to Metal/SDL3 (see `CROSS_PLATFORM_STRATEGY.md`).

### macOS: "Graphics Playground" cannot be opened

If you get a security warning on macOS:
```bash
sudo xattr -r -d com.apple.quarantine build/bin/"Graphics Playground"
```

### Linux: Missing OpenGL Libraries

```bash
# Ubuntu/Debian
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev

# Fedora
sudo dnf install mesa-libGL-devel mesa-libGLU-devel
```

### Windows: DLL Not Found

If you get "DLL not found" errors, copy required DLLs from vcpkg to the executable directory:
```cmd
copy [vcpkg-root]\installed\x64-windows\bin\*.dll build\bin\Release\
```

### CMake: Cannot find glfw3/glbinding/etc.

Ensure you've:
1. Installed dependencies via vcpkg
2. Specified the correct toolchain file: `-DCMAKE_TOOLCHAIN_FILE=...`
3. Used the correct vcpkg triplet (x64-windows, x64-osx, x64-linux)

---

## Build Options

### Debug Build

For debugging with symbols:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake \
         -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

### Build Tests (Optional)

Enable tests in CMake:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake \
         -DBUILD_TESTS=ON
cmake --build .
```

---

## IDE Integration

### Visual Studio Code

1. Install **C/C++** and **CMake Tools** extensions
2. Open project folder
3. Configure CMake kit (select your compiler)
4. Set CMake configure args in `.vscode/settings.json`:
   ```json
   {
     "cmake.configureArgs": [
       "-DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake"
     ]
   }
   ```
5. Press F7 to build


## Clean Build

To start fresh:
```bash
# Remove build directory
rm -rf build

# Recreate and rebuild
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

---

## Support

For issues:
1. Check this BUILD.md first
2. Review error messages carefully
3. Ensure all dependencies are installed via vcpkg
4. Verify CMake version (3.20+)
5. Check compiler version (C++17 support required)

Platform-specific notes:
- **macOS:** OpenGL is deprecated; expect warnings
- **Linux:** Requires X11/Wayland with OpenGL drivers
- **Windows:** Tested with Visual Studio 2019+
