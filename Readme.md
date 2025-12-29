# 3D Playground

3D Playground consists of a library component *3DEngine* which holds components to build a graphical 3D application and an executable *Graphics Playground* which creates a window, loads a scene and connects mouse and keyboard inputs. As the name suggests, this is a playground to experiment with 3D rendering concepts.

The engine uses modern OpenGL 4.1 with GLSL shaders to leverage features of modern graphics cards in order to achieve high realism while maintaining interactive frame rates.

**Cross-Platform Support:**  Builds on Windows, macOS, and Linux using CMake

## Features

* Loading of complex scenes and different model / image formats
* Very basic animation capabilities
* Multiple camera models
* Bounding volume hierarchy
* Mouse & keyboard interactivity
* Platform independent
* Supports Wavefront Obj files
* Leverages C++ 17 

### Rendering features
* Illumination from various light types (directional, spot, point, SH)
* Shadow mapping with PCF
* Multi-texturing
* Environment mapping
* Spherical Harmonics lighting

## Impressions

![Car9](https://user-images.githubusercontent.com/54601848/63808614-aa9c0980-c920-11e9-9471-32a5d2644bf6.png)

![Dragon](https://user-images.githubusercontent.com/54601848/63808650-bd164300-c920-11e9-8a4b-53430ff71c24.png)

![heli](https://user-images.githubusercontent.com/54601848/63808756-f5b61c80-c920-11e9-8495-e8752f49c64a.png)

## Prerequisites

* **CMake** 3.20 or higher
* **C++17** compatible compiler (Visual Studio 2019+, GCC 9+, Clang 10+)
* **vcpkg** for dependency management

This project now uses **CMake** for cross-platform builds and works on:
- **Windows** (Visual Studio 2019+, MSVC)
- **macOS** (Clang, Xcode)
- **Linux** (GCC, Clang)

### Dependencies

This project uses [vcpkg](https://github.com/Microsoft/vcpkg) to manage dependencies.

**Required libraries:**
* [GLFW3](https://www.glfw.org) - Window creation and input handling
* [glbinding](https://glbinding.org) - Type-safe OpenGL bindings
* [TinyXML2](https://github.com/leethomason/tinyxml2) - XML parsing
* [GLM](https://glm.g-truc.net) - OpenGL mathematics
* [stb_image](https://github.com/nothings/stb) - Image loading

**Install dependencies:**

**macOS:**
```bash
vcpkg install glfw3:x64-osx glbinding:x64-osx glm:x64-osx tinyxml2:x64-osx
```

**Linux:**
```bash
vcpkg install glfw3:x64-linux glbinding:x64-linux glm:x64-linux tinyxml2:x64-linux
```

**Windows:**
```bash
vcpkg install glfw3:x64-windows glbinding:x64-windows glm:x64-windows tinyxml2:x64-windows
```

## Building

**See [BUILD.md](BUILD.md) for detailed build instructions for each platform.**

**Quick start (all platforms):**
```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Running the Application

From the project root directory:

**macOS/Linux:**
```bash
./build/bin/"Graphics Playground" [scene.xml]
```

**Windows:**
```cmd
build\bin\Release\"Graphics Playground.exe" [scene.xml]
```

If no scene is specified, the application will list available scenes interactively.

## Platform Notes

- **[BUILD.md](BUILD.md)** - Build instructions for each platform

### macOS
- OpenGL 4.1 is supported but **deprecated** since macOS 10.14 (Mojave)
- You will see deprecation warnings - this is expected
- For long-term macOS support, consider migrating to Metal or SDL3 (see `CROSS_PLATFORM_STRATEGY.md`)
- Tested on Intel Macs; Apple Silicon may have limited OpenGL support

### Linux
- Requires OpenGL drivers and X11/Wayland
- Install mesa development libraries: `sudo apt-get install libgl1-mesa-dev`

### Windows
- Fully supported with Visual Studio 2019+
- Legacy .vcxproj files still available for direct VS builds


## License

This project is licensed under the [GNU Lesser General Public License version 3](https://opensource.org/licenses/LGPL-3.0). See LICENSE file in the project root


## Known Issues

* Rendering artifacts on Intel HD Graphics 4600
* macOS OpenGL deprecation warnings (expected behavior)