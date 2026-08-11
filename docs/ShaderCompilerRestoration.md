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
- [x] Generate RRASM metadata and VM callbacks from one shared opcode registry.
- [x] Compile and load 39 known-positive shaders through the real RRASM parser.
- [x] Fix RRASM array declarations so `array <size> <type>` consumes the element type correctly.

### Unsupported-feature diagnostics

- [x] Reject `environment()` with a source-located, explicit not-implemented diagnostic.
- [x] Reject the missing `alpha` imager global with a source-located, explicit not-implemented diagnostic.
- [x] Diagnose invalid array sizes, element types, scalar indexing, constant out-of-range indices, and non-array function arguments.

### Fixed-size float arrays

- [x] Preserve array shape in RRASM symbols and allocate storage for every SIMD block and element.
- [x] Implement scalar-float `arrld.sss` and `arrst.sss` for uniform and varying arrays and indices.
- [x] Alias inlined array function parameters to their caller arrays and validate matching lengths.
- [x] Fix chained element assignment so `a[0] = a[1] = value` reuses the assigned value.
- [x] Execute a multi-block, partial-final-block array round trip in the VM regression suite.
- [x] Compile and load the historical `test11.sl` array case through the compiler and RRASM loader.

### Visual and control-flow regression

- [x] Render two array-shaded objects selected through `if`/`else`, then assert red-left and green-right pixels.
- [x] Stage render assets in the build tree, reject renderer error output, and prevent stale images or generated source-tree files.
- [x] Fix the scalar SIMD full-mask definition that made uniform `if` conditions always false on Apple Silicon.
- [x] Add a direct VM semantic test for both uniform `if` branches.

## Next step

- [ ] Add an opt-in ASan/UBSan build and run the compiler, VM, and visual regression ladder under it.
- [ ] Decide whether to extend arrays to additional element types or first implement varying `if`/`else` lane masks.
- [ ] Add logical unary `!` once boolean and varying-mask semantics are covered.

## Correctness backlog

- [ ] Make compiler output deterministic; the first compiler-to-loader regression is now in place.
- [x] Validate opcode metadata, callback coverage, and compiler output exercised by the positive shader corpus.
- [x] Cover opcode metadata and callback alignment by generating both tables from one shared registry.
- [x] Make the known unsupported `environment()` built-in and `alpha` global compile-time errors.
- [ ] Add an opt-in ASan/UBSan test build.
- [ ] Implement logical unary `!` after verifying boolean VM semantics.
- [x] Implement fixed one-dimensional float arrays end to end.
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
4. Small deterministic renders with reference-image comparisons or focused pixel assertions.

## Known corpus gaps

- `shinimetal_test.sl` now fails with an explicit diagnostic because `environment()` remains unimplemented.
- `background.sl` now fails with an explicit diagnostic because the `alpha` imager global remains unimplemented.
- `test03.sl` deliberately calls a float function with a point argument and remains a negative type-checking case.
- Arrays currently support only `float` elements; array-valued shader/RIB parameters and other element types remain future work.
- Varying `if`/`else` lane masks remain unimplemented; the new control-flow regression covers uniform conditions.
- Matrix equality opcodes are declared but have no implementation, so the RRASM parser now rejects them instead of allowing a null callback.

## Diagnostic builds

- **AddressSanitizer (ASan)** detects memory errors at runtime, including use-after-free, out-of-bounds access, and double
  deletion.
- **UndefinedBehaviorSanitizer (UBSan)** detects many operations for which C++ defines no valid behavior, including some
  invalid shifts, arithmetic overflow, alignment errors, and invalid object access.

These sanitizers are intended for dedicated test builds. They add runtime and memory overhead and are not part of shipped
renderer binaries.
