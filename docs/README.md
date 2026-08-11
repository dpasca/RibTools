# Documentation

The maintained RibTools documentation is written in Markdown so it can be reviewed with the code and rendered directly
by common repository browsers.

## Current guides

- [Project overview and user guide](../README.md)
- [Development guide](../DEVELOPMENT.md)
- [Architecture overview](Architecture.md)
- [Shader compiler restoration tracker](ShaderCompilerRestoration.md)
- [Renderer showcase tracker](RendererShowcaseTracker.md), a temporary record of the current renderer usability work

## Images and project history

- `images/` contains the current RibRenderToy screenshot, older sample renders, and source artwork.
- `progress/` contains historical renderer-development captures from 2009 and 2010.

The former Credits, Developer Manual, and User Manual PDFs were snapshots made in 2010. They documented the original
Windows-only build, freeglut interface, and batch-file workflows, so they were retired during the 2026 maintenance work
instead of being presented as current documentation. Their original versions remain available in Git history and at the
`pre-2026-maintenance` tag. Current credits and attribution are maintained in the project README.

Generated documentation should only be checked in when it has a reproducible source and build path. If a printable guide
is needed for a future release, generate it from the maintained Markdown rather than creating a separate source of truth.
