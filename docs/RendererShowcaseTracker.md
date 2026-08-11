# Renderer Showcase Tracker

This temporary document tracks renderer usability and showcase work separately from shader-compiler restoration. Timings
below are diagnostic observations from an Apple Silicon ASan/UBSan build, not performance benchmarks.

## Completed foundation

- [x] Audit the principal bundled complex scenes at low resolution.
- [x] Run complex texture and displacement rendering under ASan/UBSan.
- [x] Add camera-relative turn, pan, dolly, reset, and optional field-of-view controls to RibRenderToy.
- [x] Render camera edits at a configurable low-resolution preview before an explicit full-resolution render.
- [x] Cover camera-matrix composition and field-of-view overrides with a direct regression test.
- [x] Add a low-resolution Killeroo regression with a tolerant luminance-and-silhouette signature.

## Scene audit

| Scene | Diagnostic result | Current role |
| --- | --- | --- |
| Killeroo NURBS | Completed at 128 pixels in 2.82 seconds after the texture-kernel fix. Its unsupported `Imager` request is now documented and disabled, leaving the intended black showcase background without an error. | Strong showcase candidate because it exercises NURBS, displacement, and textures. |
| Dragonhead | Completed at 256 pixels in 24.65 seconds with no sanitizer finding. The model is coherent, but the default framing is small against a black background. | Dense bicubic-patch alternative to compare with Killeroo after presentation polish. |
| Enterprise | Completed at 256 pixels in 0.64 seconds, but rejects `ShadingRate` and both legacy light definitions, leaving an almost-black image. | Compatibility case, not currently a showcase. |
| Sponza | A 128-pixel sanitizer render was stopped after 291.53 seconds without completing a frame. Its referenced custom shaders and ten texture maps are also absent. | Heavy geometry and parser stress case. |

## Next renderer steps

- [ ] Trace the upstream source of the non-finite shaded sample seen in the 160-pixel Killeroo sanitizer render;
  integer image conversion is now safe and deterministic, but the originating calculation remains unknown.
- [ ] Improve Killeroo and Dragonhead framing, background, and lighting, then compare fixed showcase renders before choosing a hero scene.
- [ ] Evaluate any vehicle or reference scene for licensing, RIB compatibility, shaders, and texture assets.
- [ ] Report scene errors structurally so RibRenderToy does not describe a partially failed render as successful.
- [ ] Add cancellable or asynchronous preview rendering if camera iteration remains too slow on larger scenes.
- [ ] Render and export ordinary multi-frame camera or object animation sequences.
- [ ] Implement sampled `MotionBegin`/`MotionEnd` transforms only after frame animation is reliable.
