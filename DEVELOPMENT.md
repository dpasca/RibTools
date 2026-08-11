# Development

## C++ and Objective-C++ style

New and substantially edited hand-written C++ and Objective-C++ code follows
[Davide Pasca's current C++ style guide](https://gist.githubusercontent.com/dpasca/5243278/raw/c92097874494f1735e033e1937506701f747dc4d/CodeStyle.cpp).
In particular:

- Indent with four spaces. Do not add tab characters.
- Use Allman braces.
- Favor `const` and `auto` when they make intent clearer.
- Initialize scalar and POD members explicitly.
- Prefer standard containers and RAII ownership.
- Reduce nesting with early exits, use `size_t` for sizes and indices, and prefer pre-increment when the old value is not needed.

This policy is forward-looking. Do not mass-format legacy code or combine unrelated whitespace churn with a functional
change. Format new files freely; in existing files, limit formatting to the lines or self-contained blocks being changed.

The repository includes `.editorconfig` and `.clang-format` files to make the mechanical parts repeatable. Run
`clang-format` only on new files or deliberately selected changed ranges unless a standalone formatting change has been
agreed in advance. Vendored and archival directories are excluded through `.clang-format-ignore`.

## Changes

Keep commits focused and preserve existing public interfaces unless a change explicitly requires an API break. Generated,
vendored, shader, and third-party code should retain its native style.

## Diagnostic builds

Clang and GCC builds can opt into AddressSanitizer and UndefinedBehaviorSanitizer through the dedicated preset:

```sh
cmake --preset sanitize
cmake --build --preset sanitize
ctest --preset sanitize
```

The instrumented executables are written to `_build/sanitize/bin` so they do not replace the normal binaries in
`_distrib`. Runtime assets are staged there automatically. Sanitizer findings stop the failing test immediately.

## Visual regression tests

The test suite renders its image fixtures from isolated runtime copies, so compiling their shaders does not modify the
source tree. The Killeroo check forces the bundled scene to 160 pixels on its longest edge and compares a coarse
luminance-and-alpha signature. This tolerates small platform differences while still catching missing geometry, changed
framing, and substantial shading or displacement regressions.

CI retains each platform's `killeroo.0001.tif` as a downloadable artifact. After an intentional visual change, inspect
the TIFF first, then use the checker to print reviewed replacement values:

```sh
_distrib/RIKillerooVisualTests _build/release/killeroo.0001.tif --print-reference
```

## Runtime staging and packaging

CMake stages `DistribSrc/base` into the selected runtime output directory whenever the applications are built. Do not add
platform-specific copy scripts; Windows, macOS, and Linux use the same CMake target.

Generated `*.autogen.rrasm` shader caches deliberately survive ordinary builds. Remove them with:

```sh
cmake --build --preset release --target RibToolsCleanShaderCache
```

To validate the distributable layout, build the Release preset and use its CPack target:

```sh
cmake --preset release
cmake --build --preset release --parallel
cmake --build --preset release --target package
```

The resulting ZIP and SHA-256 checksum are placed in `_build/release/packages`. Packaging uses CMake's clean temporary
install tree rather than copying `_distrib`.
