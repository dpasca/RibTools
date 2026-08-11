# Shader Compiler Restoration Tracker

This is a temporary working document for restoring the RSL compiler and shader VM. It should remain concise and can be
removed or converted into permanent documentation once the work is complete.

## Working principles

- Fix correctness and establish regression coverage before adding language features.
- Keep each change small enough to review and test independently.
- Modernize toward C++20 incrementally without mass-formatting historical code.
- Prefer explicit ownership, including `std::unique_ptr`, when changing ownership-sensitive code.
- Prefer raw string literals for embedded multiline test source when they improve readability.
- Require compiler output to be accepted and executed by the real shader runtime.

## Completed steps

### Preprocessor correctness

- [x] Replace borrowed preprocessor macro-name pointers with owned strings.
- [x] Preserve quoted strings during macro expansion.
- [x] Keep nested conditional branches inactive when their parent is inactive.
- [x] Add focused macro and conditional regression tests.
- [x] Re-enable the historical preprocessor shader as an integration test.

### Unary arithmetic

- [x] Distinguish prefix `+` and `-` from their binary forms using expression context rather than whitespace.
- [x] Give unary operators their own right-to-left AST nodes and precedence above binary arithmetic.
- [x] Type-check scalar and three-component unary expressions and fold numeric constants.
- [x] Add scalar and vector `neg` instructions to the RRASM runtime instead of synthesizing an invalid literal-zero operand.
- [x] Add focused, historical-shader, and compiler-to-RRASM-loader regression coverage.

### Compiler and VM opcode contract

- [x] Verify identical table lengths and require callbacks at every executable opcode index.
- [x] Require unique case-insensitive names, valid operand counts and types, known flags, and callbacks for executable opcodes.
- [x] Mark VM-handled and deliberately unimplemented instructions explicitly; reject unimplemented instructions while loading.
- [x] Compile and load 35 known-positive shaders through the real RRASM parser.
- [x] Fix RRASM array declarations so `array <size> <type>` consumes the element type correctly.

## Next step

- [ ] Replace the parallel opcode arrays with one shared registry when making the next structural VM change.
- [ ] Improve diagnostics for unsupported globals and built-ins before starting fixed-size array work.
- [ ] Implement and test the missing `arrld` and `arrst` VM instructions.

## Correctness backlog

- [ ] Make compiler output deterministic; the first compiler-to-loader regression is now in place.
- [x] Validate opcode metadata, callback coverage, and compiler output exercised by the positive shader corpus.
- [ ] Cover unexercised opcode-emission paths by generating the tables from one shared registry.
- [ ] Make unsupported built-ins compile-time errors instead of emitting unusable code.
- [ ] Add an opt-in ASan/UBSan test build.
- [ ] Implement logical unary `!` after verifying boolean VM semantics.
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

## Known corpus gaps

- `shinimetal_test.sl` now compiles past `-I`, then fails while resolving its unsupported `environment()` expression.
- `background.sl` uses an `alpha` imager global that is not declared by the compiler built-ins or runtime.
- `test03.sl` deliberately calls a float function with a point argument and remains a negative type-checking case.
- `test11.sl` compiles array operations and its declarations now load correctly, but the VM lacks `arrld` and `arrst`.
- Matrix equality opcodes are declared but have no implementation, so the RRASM parser now rejects them instead of allowing a null callback.

## Diagnostic builds

- **AddressSanitizer (ASan)** detects memory errors at runtime, including use-after-free, out-of-bounds access, and double
  deletion.
- **UndefinedBehaviorSanitizer (UBSan)** detects many operations for which C++ defines no valid behavior, including some
  invalid shifts, arithmetic overflow, alignment errors, and invalid object access.

These sanitizers are intended for dedicated test builds. They add runtime and memory overhead and are not part of shipped
renderer binaries.
