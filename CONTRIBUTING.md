# Contributing

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
