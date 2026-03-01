# eHaz Game Engine

## Description

eHaz is a platform-agnostic, modular, and open-source game engine
written in C++.\
It is designed to be easily modifiable and extensible, giving developers
full control over their technology stack without restrictive licensing,
pricing models, or technological lock-ins.

The engine focuses on flexibility, transparency, and suitability for
both 2D and lightweight 3D applications.

### Core Systems

-   Renderer
-   Entity Component System (ECS)
-   Scene System
-   Physics System
-   UI System
-   Asset Manager
-   Event Queue
-   Input System
-   Spatial Data Structures (Octree)
-   PBR Rendering (Cook-Torrance)

------------------------------------------------------------------------

## Dependencies

### Core Technologies

-   C++
-   OpenGL
-   SDL3
-   EnTT
-   Assimp
-   CMake
-   Boost C++ Libraries
-   Jolt Physics
-   Dear ImGui
-   GLM
-   GLAD

### Linux (Ubuntu 25.10)

``` bash
sudo apt update
sudo apt install cmake
sudo apt install libboost-all-dev
sudo apt install libassimp-dev
sudo apt install clang
```

### Windows (VCPKG)

Requires CMake.\
Clang is recommended, but MSVC is supported.

``` bash
vcpkg install boost
vcpkg install assimp:x64-windows-static
```

------------------------------------------------------------------------

## Installation

1.  Clone the repository:

``` bash
git clone https://github.com/FloatZ1/eHaz-Game-Engine
```

2.  Install dependencies (see above).

3.  Clone the renderer dependency:

Clone EnvHazGraphics into:

    Engine/dependencies/EnvHaz/

4.  Install:
    -   GLAD into:

            EnvHaz/EnvHaz-Graphics/lib/dependencies/glad/

    -   GLM into:

            EnvHaz/EnvHaz-Graphics/lib/dependencies/glm/glm/
5.  Configure and build using CMake:

``` bash
cmake -S . -B build
cmake --build build
```

------------------------------------------------------------------------

## Small Example

Minimal application setup:

``` cpp
int main() {

  eHaz_Core::AppSpec spec;
  spec.fullscreen = false;
  spec.title = "eHaz Game Engine";
  spec.w_width = 1280;
  spec.w_height = 720;

  eHaz_Core::Application app(spec);

  app.push_layer<eHaz::EditorUILayer>(true);
  app.push_layer<AppLayer>();

  app.Run();
}
```

Creating a custom layer:

``` cpp
class AppLayer : public eHaz::Layer { 

  void OnCreate() override { }
  void OnUpdate(float dt) override { }
  void OnEvent(Event& event) override { }
  void OnRender() override { }

};
```

The engine core interface is located in `Application.hpp`.\
User code interacts with the engine through the layer system, allowing
clean separation between engine functionality and gameplay logic.
