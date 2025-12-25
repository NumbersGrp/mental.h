# Mental SDK

A C++ SDK for window management and OpenGL rendering using GLFW and GLEW.

## Features

- **Template-based architecture** for flexible renderer integration
- **GLFW window management** with OpenGL context setup
- **GLEW integration** for OpenGL extension loading
- **Header-only library** for easy integration
- **Comprehensive Doxygen documentation**

## Dependencies

- **GLFW 3.x** - Window and input handling
- **GLEW** - OpenGL extension loading
- **GLM** - OpenGL Mathematics library
- **OpenGL 3.3+** - Graphics rendering
- **CMake 3.16+** - Build system

## Installation

### macOS
```bash
brew install glfw glew glm
```

### Ubuntu/Debian
```bash
sudo apt-get install libglfw3-dev libglew-dev libglm-dev
```

### Fedora
```bash
sudo dnf install glfw-devel glew-devel glm-devel
```

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```cpp
#include "sdk.hpp"

int main() {
    // Configure window
    mentalsdk::WindowManagerInfo info{};
    info.aSizes[0] = 1024;
    info.aSizes[1] = 768;
    info.title = "My Application";
    
    // Create window manager with renderer
    mentalsdk::CMentalWindowManager<
        mentalsdk::GLFWwindow, 
        mentalsdk::CMentalRenderer<int>
    > window(info);
    
    // Start main loop
    window.run();
    
    return 0;
}
```

## Documentation

Generate HTML documentation using Doxygen:

```bash
doxygen Doxyfile
```

The documentation will be generated in the `docs/html/` directory. Open `docs/html/index.html` in your browser to view it.

### Documentation Features

- **Class diagrams** showing inheritance and relationships
- **Detailed API reference** with parameter descriptions
- **Usage examples** for all major components
- **Template parameter documentation**
- **Exception specifications**

## Architecture

### Core Components

- **CMentalWindowManager** - Template-based window management
- **CMentalRenderer** - Template-based OpenGL renderer
- **WindowManagerInfo** - Window configuration structure

### Template Design

The SDK uses templates to allow custom renderer implementations:

```cpp
template <typename WindowType, class RendererType>
class CMentalWindowManager;

template <class PipelineType>
class CMentalRenderer;
```

This design enables:
- Custom rendering pipelines
- Flexible window backends
- Type-safe integration
- Compile-time optimization

## Project Structure

```
SDK/
├── sdk.hpp                     # Main SDK header
├── example.cpp                 # Usage example
├── Renderer/
│   ├── CMentalRenderer.hpp     # Renderer component
│   └── CMentalRenderer.cpp     # (Legacy - now header-only)
└── WindowManager/
    ├── CMentalWindowManager.hpp # Window manager component
    └── CMentalWindowManager.cpp # (Legacy - now header-only)
```

## License

[Add your license information here]

## Contributing

[Add contribution guidelines here]