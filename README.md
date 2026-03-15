# Graph Traversal Visualizer

A modular C++ application for visualizing pathfinding and graph traversal algorithms.

## Features
- Multi-threaded search execution for real-time UI animation.
- Support for grid-based mazes and general adjacency list graphs.
- Smooth edge-filling animations during node discovery.
- Path reconstruction and highlighting.
- Interactive control panel for algorithm selection and speed control.

## Architecture
- Strategy Pattern: Algorithm logic decoupled from frontier data structures.
- Observer Pattern: Lambda hooks for thread-safe state reporting and rendering.
- Modern C++: Utilization of RAII, smart pointers (unique_ptr), and atomics.
- Thread Safety: State synchronization via std::mutex and std::lock_guard.
- Generic Rendering: Graph-agnostic pipeline using delegated rendering hooks.

## Tech Stack
- Graphics: Raylib
- UI: Dear ImGui (via rlImGui)
- Build System: CMake 3.11+

## Getting Started

### Build Requirements
- C++17 Compiler
- CMake
- OpenGL & X11 development headers (Linux)

### Compilation
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./GraphTraversalVisualizer
```
