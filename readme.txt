//==================================================================
/// RibTools - http://kazzuya.com/ribtools
//==================================================================

* About

RibTools is a hobby project developed by me (Davide Pasca).
The project main goal is to write RibRender, a RenderMan-compliant
renderer.
The focus of RibRender is towards speed and high scalability, it
aims to achieve this by utilizing parallelism at all levels:
 
 - Instruction level parallelism (Adaptive N-way SIMD)
 - Many-cores hardware (OpenMP)
 - Network-distributed rendering (Custom protocol over TCP/IP)

For more details, please see the project's web site, or contact
me personally:

 - web: http://kazzuya.com/ribtools
 - e-mail: dpasca <at> gmail.com


* Building

- Use the build.bat script (currently DOES NOT build EXEs)
- The PC Version requires Visual Studio 2008 or so


* Credits

RibTools includes code from the following libraries:

- JPEG library - http://www.ijg.org
- freeglut - http://freeglut.sourceforge.net

Sample files were found on the Internet and are assumed to be
freely redistributable. For any issues in that regard, please
contact me personally and I'll be happy to give due credit or
remove whatever eventual material 8)

Most of the included RIB files are old samples that I found
included in the "RenSH"(1) project.
The "Sponza Atrium" model is originally from Marko Dabrovic. The
RIB version was taken from the "lucille"(2) renderer project.

- (1) RenSH - http://www.dctsystems.co.uk/RenderMan/rensh.html
- (2) lucille - http://lucille.sourceforge.net
