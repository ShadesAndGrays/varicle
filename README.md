<div align="center">

# Varicle Framework

*How I spend my free time? Coding my own engine, of course.*

<img src="assets/bird.png" width="220" alt="Varicle Logo / Mascot" />

---

</div>

Varicle is a code-first, experimental game/graphics engine written in C++23. The
long-term goal is a small engine core that can drive the same game code across
multiple graphics backends instead of locking a project to a single rendering
API.

It is primarily a personal playground: architecture experiments, renderer work,
and side projects collected into one place.

## A Quick Heads-Up

- **Work in progress:** module boundaries and APIs are still moving.
- **Expect redundancy:** some parts are over-engineered, unused, or being
  reworked. That is part of the experiment.
- **Backends are not equally complete:** Vulkan is the active renderer. OpenGL
  and WebGPU have source in the tree but are not wired into the build yet.
- **Will it ever be a complete engine?** *I have no idea.*

## AI Usage

Most of the code in this project is human-written. AI coding agents are used
selectively as a supporting tool, typically for small refactors, boilerplate,
math helpers, tests, or documentation. The architecture, renderer work, and
design decisions come from me.

AI-assisted changes are reviewed and adapted before being kept. Some commits
contain a mix of hand-written and AI-assisted code, so the history is best read
as an experiment in practical, human-directed AI assistance rather than
AI-generated engine development.

## Renderer Backends

| Backend | Status | Notes |
| :--- | :--- | :--- |
| **Vulkan** | Active | Main renderer; uses vulkan-hpp, GLFW, Slang, and KTX |
| **OpenGL** | Experimental | Source present under `renderer/opengl`, currently disabled |
| **WebGPU** | Experimental | Source present under `renderer/webgpu`, not integrated |

The shared renderer layer lives in `renderer/common` and is intended to stay
backend-agnostic where practical. The engine also has an internal type and math
layer so rendering code can avoid leaking backend-specific math types.

## Stack & Dependencies

| Tool | Purpose |
| :--- | :--- |
| **C++23 + CMake** | Language and build system |
| **GLFW** | Window creation and input |
| **Vulkan SDK / vulkan-hpp** | Active graphics backend |
| **Slang** | Shader compilation |
| **KTX** | Texture container support |
| **EnTT** | Entity Component System |
| **Box2D** | 2D physics |
| **doctest** | Unit testing |
| **glm** | Still used in parts of the renderer; being replaced by core math |
| **raylib / ImGui** | Vendored legacy tooling; raylib is mostly archived |

Most dependencies are vendored under `vendor/`. A Vulkan SDK installation is
required for the default build.

## Core Modules

| Path | Description |
| :--- | :--- |
| `core/` | Engine types, math, colors, events, service locator, slot maps |
| `renderer/common/` | Backend-independent window, camera, handles, and vertex code |
| `renderer/vulkan/` | Vulkan backend, resource manager, pipelines, and cameras |
| `renderer/opengl/` | In-progress OpenGL backend |
| `renderer/webgpu/` | In-progress WebGPU backend |
| `engine/` | In-tree ECS, scenes, assets, physics, and UI modules |
| `examples/` | Sample applications, including the active `vulkan-render` example |
| `tests/` | Internal unit tests built with doctest |
| `archive/` | Older build setup and retired experiments |

The `engine/` modules are in the repository but are not all wired into the
current top-level CMake build while the renderer integration is being reworked.

## Getting Started

### Prerequisites

- A C++23 compiler
- CMake 3.29 or newer
- Python 3 for the build helper
- Vulkan SDK 1.4.x for the default Vulkan build
- Ninja (recommended) or another CMake-supported generator

### Build with `build.py`

`build.py` wraps the CMake configure, build, and optional run steps. It also
remembers options in `.build_cache.json` between runs.

```bash
python build.py build --debug --examples --tests --parallel
```

Useful flags:

| Flag | Effect |
| :--- | :--- |
| `--debug` | Build with `CMAKE_BUILD_TYPE=Debug` |
| `--examples` | Enable example applications |
| `--tests` | Enable internal unit tests |
| `--parallel` | Build in parallel |
| `--run <path>` | Run an executable after building |
| `--skipconfig` | Skip the CMake configure step |
| `--nodebug` | Return to a release build |
| `--noexamples` | Disable examples for subsequent cached builds |
| `--notests` | Disable tests for subsequent cached builds |
| `--norun` | Do not run a target after building |

### Build with CMake directly

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_TESTS=ON

cmake --build build --parallel
```

### Run an example

```bash
python build.py build --examples --run ./build/examples/vulkan-render/vulkan-render.exe
```

### Run the tests

```bash
ctest --test-dir build --output-on-failure
```

To run only the math tests:

```bash
ctest --test-dir build -R math_test --output-on-failure
```

## Current Focus

- Building a backend-agnostic renderer interface around the Vulkan implementation
- Growing the core math layer (`Mat2`/`Mat3`/`Mat4`, transforms, projections)
- Replacing remaining `glm` usage in renderer-facing code with core types
- Expanding internal tests before re-integrating the engine modules
