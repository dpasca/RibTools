# RibTools Architecture

This document is a high-level map of the current codebase. It describes responsibilities and data flow rather than
serving as a fixed API contract; much of the implementation intentionally retains the shape of the original renderer.

## Render pipeline

```text
RIB scene
   |
   v
RibRender / RibRenderToy
   |
   v
RibRenderLib
   RI::Parser -> RRL::Translator -> RI::State
                                      |
                                      v
                                RI::Framework
                                      |
                                      v
                    split -> dice -> shade -> hide
                                      |
                                      v
                    file display or interactive display

.sl shader -> RSLCompilerLib -> .rrasm -> RI_System shader VM
```

`RRL::Render` reads a RIB file and uses `RI::Parser` to turn it into commands. `RRL::Translator` validates and dispatches
those commands to `RI::State`, which tracks options, attributes, transformations, lights, shaders, and primitives.

At `WorldEnd`, `RI::Framework` prepares displays and buckets. The REYES-style hider splits complex primitives, dices them
into micropolygon grids, evaluates shaders through the shader virtual machine, and rasterizes the resulting
micropolygons. Display drivers then copy the image to a file or an OpenGL-backed interactive view.

Shaders use a separate path. `RSLCompilerLib` preprocesses and compiles RenderMan-like `.sl` source into RibTools'
`.rrasm` instruction format. `RI_System` parses and executes that format. The compiler and virtual machine therefore share
an opcode contract that is covered by the shader compiler regression tests.

## Main components

| Component | Responsibility |
| --- | --- |
| `DSystem` | Low-level containers, strings, file access, threading, synchronization, networking, allocation, and portability helpers. |
| `DMath` | Scalar and SIMD vector math, matrices, splines, and random-number utilities. |
| `DImage` | In-memory images plus BMP, JPEG, PPM, and TIFF input/output. |
| `RSLCompilerLib` | Shader preprocessing, tokenization, syntax and type handling, and `.rrasm` generation. |
| `RI_System` | RenderMan-like state, geometry, lights, textures, shader VM, dicing, micropolygon generation, bucket rendering, and output buffers. |
| `RibRenderLib` | RIB rendering orchestration, command translation, resource setup, and experimental distributed-rendering support. |
| `DispDrivers` | File and OpenGL framebuffer display adapters compiled into the interactive applications. |
| `RibToolsBase` | Locates the runtime directory and its `ribtools.ini` marker. |
| `RibToolsUI` | Shared Dear ImGui and GLFW application host. |
| `DistribSrc/base` | Canonical runtime shaders, scenes, textures, and the runtime marker file. |

The pinned third-party dependencies under `externals` provide libjpeg-turbo, LibTIFF, Dear ImGui, and GLFW. CMake builds
the image libraries statically and stages `DistribSrc/base` beside the applications.

## Applications

| Application | Role |
| --- | --- |
| `RibRenderToy` | Interactive scene browser and renderer with camera controls and diagnostic views. |
| `RibRender` | Command-line renderer and optional client for distributed bucket rendering. |
| `RibRenderServer` | Experimental network worker used by `RibRender`. |
| `RSLCompilerCmd` | Standalone shader compiler frontend used for testing and diagnostics. |

`RibRender` and `RibRenderToy` share rendering behavior through `RibRenderLib`. The user-interface layer should remain
separate from renderer state so command-line and regression-test paths can exercise the same core behavior.

## Runtime and generated data

- `_build/<preset>` contains CMake build trees and test outputs.
- `_distrib` is the default development runtime directory. It is generated and should not contain source changes.
- `_build/sanitize/bin` is the isolated runtime directory for ASan/UBSan builds.
- `*.autogen.rrasm` files are shader caches generated beside runtime shader sources. They may be removed with the
  `RibToolsCleanShaderCache` CMake target.
- CPack creates release archives from a fresh install tree rather than copying `_distrib`.

## Tests and maintenance boundaries

CTest covers utility libraries, shader preprocessing and compilation, the compiler/VM opcode contract, texture sampling,
camera overrides, executable startup, and a small rendered-image check. ASan/UBSan runs the same suite with runtime
instrumentation.

Renderer changes should generally be tested at the lowest useful boundary: direct library tests for state and math,
compiler tests for language behavior, and small deterministic renders when behavior only becomes visible after the full
pipeline. Large scenes such as Killeroo and Dragonhead remain valuable diagnostic and presentation cases, but should not
replace focused regression tests.

The `RI_System/old` directory contains inactive historical implementations and is not part of the normal build. Avoid
treating it as a current alternative implementation unless a restoration task explicitly calls for it.
