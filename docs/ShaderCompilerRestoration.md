# Shader Compiler Restoration Tracker

This is a temporary working document for restoring the RSL compiler and shader VM. It should remain concise and can be
removed or converted into permanent documentation once the work is complete.

## Working principles

- Fix correctness and establish regression coverage before adding language features.
- Keep each change small enough to review and test independently.
- Modernize toward C++20 incrementally without mass-formatting historical code.
- Prefer explicit ownership, including `std::unique_ptr`, when changing ownership-sensitive code.
- Require compiler output to be accepted and executed by the real shader runtime.

## Completed steps

### Preprocessor correctness

- [x] Replace borrowed preprocessor macro-name pointers with owned strings.
- [x] Preserve quoted strings during macro expansion.
- [x] Keep nested conditional branches inactive when their parent is inactive.
- [x] Add focused macro and conditional regression tests.
- [x] Re-enable the historical preprocessor shader as an integration test.

## Next step

- [ ] Add focused regressions for unary `+` and `-` and subtraction without surrounding whitespace.
- [ ] Fix operator parsing without changing established binary-expression behavior.
- [ ] Recompile the affected historical and scene shaders.

## Correctness backlog

- [ ] Make compiler output deterministic and test it through the RRASM loader.
- [ ] Implement unary operators and subtraction without surrounding whitespace.
- [ ] Validate that every emitted built-in opcode has a matching VM implementation.
- [ ] Make unsupported built-ins compile-time errors instead of emitting unusable code.
- [ ] Add an opt-in ASan/UBSan test build.
- [ ] Implement fixed one-dimensional arrays end to end.
- [ ] Implement varying `if`/`else` lane masks, followed by loop control flow.

## Modernization backlog

- [ ] Introduce structured compiler results and diagnostics.
- [ ] Separate parsing, semantic analysis, intermediate representation, and assembly emission.
- [ ] Replace owning AST pointers with `std::unique_ptr` or a stable arena in focused stages.
- [ ] Replace legacy nulls and casts as nearby code is changed.
- [ ] Add liveness-based temporary-register reuse after correctness coverage is in place.

## Test ladder

1. Unit tests for compiler stages such as preprocessing and expression handling.
2. Compile-and-load tests that pass every emitted shader through the RRASM parser.
3. Semantic tests that execute shaders over small, controlled sample grids.
4. Small deterministic renders with reference, result, and difference images.

## Diagnostic builds

- **AddressSanitizer (ASan)** detects memory errors at runtime, including use-after-free, out-of-bounds access, and double
  deletion.
- **UndefinedBehaviorSanitizer (UBSan)** detects many operations for which C++ defines no valid behavior, including some
  invalid shifts, arithmetic overflow, alignment errors, and invalid object access.

These sanitizers are intended for dedicated test builds. They add runtime and memory overhead and are not part of shipped
renderer binaries.
