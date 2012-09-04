RibTools
========

**RibTools** is an experiment in writing an off-line renderer based on Pixar's [RenderMan interface] and the [REYES] rendering architecture.

[RenderMan interface]: http://en.wikipedia.org/wiki/RenderMan_Interface_Specification
[REYES]: http://en.wikipedia.org/wiki/Reyes_rendering

At the core of **RibTools** there is **RibRender**, the rendering engine, built with speed and scalability in mind.

Parallelism is implemented at all levels:
* Instruction level (adaptive N-way SIMD)
* Many-core hardware (multi-threading)
* Network-distributed rendering (custom protocol over TCP/IP)

**RibTools** is a rather crude experiment and it's only tested against a small selection of simple scenes included in the project.


Building & running
------------------

Instructions for Windows and Visual Studio.

1) Go to `Sources/` and launch `make_build.bat`

2) Open `build/RibTools.sln` with Visual Studio

3) Choose the *Release* build (*Debug* may be too slow)

4) Right click on *RibRenderToy* and choose *Set as StartUp Project*

5) Right click on *RibRenderToy* then *Properties -> Debugging -> Working Directory* and set it to `..\..\Distrib`

6) Run

7) Right click in the window to load a test object

That's all !

