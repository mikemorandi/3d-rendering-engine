# 3DEngine Static Library
add_library(3DEngine STATIC
    # Animation
    src/animation/TimeObserver.h

    # Camera
    src/camera/Camera.h
    src/camera/Camera.cpp
    src/camera/CameraAdapter.h
    src/camera/FirstPersonCameraAdapter.h
    src/camera/FirstPersonCameraAdapter.cpp
    src/camera/Frame.h
    src/camera/Frame.cpp
    src/camera/Frustum.h
    src/camera/Frustum.cpp
    src/camera/InspectionCameraAdapter.h
    src/camera/InspectionCameraAdapter.cpp
    src/camera/OrthogonalCamera.h
    src/camera/OrthogonalCamera.cpp
    src/camera/PerspectiveCamera.h
    src/camera/PerspectiveCamera.cpp

    # Core
    src/core/gl.h
    src/core/GLErrorHandler.h
    src/core/GLErrorHandler.cpp
    src/enums.h
    src/error.h

    # Input
    src/input/InputHandler.h
    src/input/KeyboardObserver.h
    src/input/MouseObserver.h
    src/input/ViewportObserver.h
    src/input/WindowEventHandler.h
    src/input/WindowEventHandler.cpp

    # Light
    src/light/AmbientLight.h
    src/light/AmbientLight.cpp
    src/light/DirectionalLight.h
    src/light/DirectionalLight.cpp
    src/light/Light.h
    src/light/Light.cpp
    src/light/LightModel.h
    src/light/LightModel.cpp
    src/light/PointLight.h
    src/light/PointLight.cpp
    src/light/SpotLight.h
    src/light/SpotLight.cpp

    # Materials
    src/materials/Material.h
    src/materials/Material.cpp

    # Math
    src/math/BoundingBox.h
    src/math/BoundingBox.cpp
    src/math/BoundingBoxUtil.h
    src/math/BoundingBoxUtil.cpp
    src/math/HaltonSphere.h
    src/math/HaltonSphere.cpp
    src/math/Ray.h
    src/math/Ray.cpp

    # Rendering
    src/rendering/GeometryBuffer.h
    src/rendering/GeometryBuffer.cpp
    src/rendering/Renderable.h
    src/rendering/Renderer.h
    src/rendering/Renderer.cpp
    src/rendering/Viewport.h
    src/rendering/Viewport.cpp

    # Scene
    src/scene/Scene.h
    src/scene/Scene.cpp
    src/scene/SceneOverlay2D.h
    src/scene/SceneOverlay2D.cpp
    src/scene/SceneParser.h
    src/scene/SceneParser.cpp

    # Shader
    src/shader/ConstColorShader.h
    src/shader/ConstColorShader.cpp
    src/shader/DepthMapShader.h
    src/shader/DepthMapShader.cpp
    src/shader/GBufferShader.h
    src/shader/GBufferShader.cpp
    src/shader/GLSLProgram.h
    src/shader/GLSLProgram.cpp
    src/shader/IntrinsicColorShader.h
    src/shader/IntrinsicColorShader.cpp
    src/shader/MaterialShader.h
    src/shader/PhongShader.h
    src/shader/PhongShader.cpp
    src/shader/ShadowDepthShader.h
    src/shader/ShadowDepthShader.cpp
    src/shader/ShaderBase.h
    src/shader/ShaderBase.cpp
    src/shader/ShaderLibrary.h
    src/shader/ShaderLibrary.cpp
    src/shader/SHDiffuseShader.h
    src/shader/SHDiffuseShader.cpp
    src/shader/SkyboxShader.h
    src/shader/SkyboxShader.cpp
    src/shader/UniformBuffer.h
    src/shader/UniformBuffer.cpp
    src/shader/UniformBufferShader.h
    src/shader/UniformBufferShader.cpp

    # Shape
    src/shape/Box.h
    src/shape/Box.cpp
    src/shape/RenderMesh.h
    src/shape/RenderMesh.cpp
    src/shape/Shape.h
    src/shape/Shape.cpp
    src/shape/ShapeInstance.h
    src/shape/ShapeInstance.cpp
    src/shape/Skybox.h
    src/shape/Skybox.cpp
    src/shape/Triangle.h
    src/shape/Triangle.cpp
    src/shape/WireCone.h
    src/shape/WireCone.cpp
    src/shape/WireCube.h
    src/shape/WireCube.cpp

    # Texture
    src/texture/CubeMapTexture.h
    src/texture/CubeMapTexture.cpp
    src/texture/DepthTexture.h
    src/texture/DepthTexture.cpp
    src/texture/Framebuffer.h
    src/texture/Framebuffer.cpp
    src/texture/ImageUtil.h
    src/texture/ImageUtil.cpp
    src/texture/Texture.h
    src/texture/Texture.cpp
    src/texture/Texture2D.h
    src/texture/Texture2D.cpp
    src/texture/Texture3D.h
    src/texture/Texture3D.cpp

    # Util
    src/util/ObjLoader.h
    src/util/ObjLoader.cpp
    src/util/RawMesh.h
    src/util/RawMesh.cpp
    src/util/ShCoeffParser.h
    src/util/ShCoeffParser.cpp
    src/util/SharedPointer.h
    src/util/TimeManager.h
    src/util/TimeManager.cpp
    src/util/Util.h
    src/util/Util.cpp
    src/util/XmlUtils.h
    src/util/XmlUtils.cpp

)

# Set target properties
set_target_properties(3DEngine PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories
target_include_directories(3DEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

# Link libraries
target_link_libraries(3DEngine
    PUBLIC
        glfw
        glbinding::glbinding
        glbinding::glbinding-aux
        glm::glm
        tinyxml2::tinyxml2
        ${PLATFORM_LIBS}
)

# Compiler warnings
if(MSVC)
    target_compile_options(3DEngine PRIVATE /W3)
    # Disable specific MSVC warnings for glbinding
    target_compile_options(3DEngine PRIVATE /wd4251)
else()
    target_compile_options(3DEngine PRIVATE -Wall -Wextra -Wpedantic)
    # Suppress deprecation warnings for OpenGL on macOS
    if(APPLE)
        target_compile_options(3DEngine PRIVATE -Wno-deprecated-declarations)
    endif()
endif()



# Platform-specific definitions
if(WIN32)
    target_compile_definitions(3DEngine PRIVATE WIN32 _LIB)
endif()

# Install rules (optional)
install(TARGETS 3DEngine
    EXPORT 3DEngineTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include/3DEngine
    FILES_MATCHING PATTERN "*.h"
)

# Add tests subdirectory (only if testing is enabled)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
