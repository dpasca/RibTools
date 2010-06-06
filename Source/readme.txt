====================
Building RibTools
====================

Requirements
============

*RibTools* currently only compiles for Windows, although the code is written with portability in mind and Windows-specific code is kept to a minimum.

On Windows the required tools are:

 * Visual Studio 2008
 * CMake_ 2.8 or better

.. _CMake: http://www.cmake.org

The Code
===========

*RibTools* is released under the `New BSD license`_

.. _New BSD license: http://www.opensource.org/licenses/bsd-license.php 

The code is officially located at http://ribtools.googlecode.com .

Directory Structure
===================
As obtained from the Subversion repository:

**Source/**

======================  =======================================================
**DImage/**             Bitmap images library
**DispDrivers/**        Display drivers sources
**DistribSrc/**         Source assets
**DMath/**              Math library sources and project
**docs/**               Documentation related files (currently just some images)
**DSystem/**            Base system library sources and projects
**external/**           External libraries, directly included
**externals/**          External libraries included automatically by SVN
**RI_System/**          Core rendering engine and RenderMan-interface
**RibRender/**          RibRender application project and sources
**RibRenderLib/**       A library that groups most functionalities used by both *RibRender *and *RibRenderToy*
**RibRenderServer/**    RibRenderServer application project and sources
**RibRenderToy/**       RibRenderToy application project and sources
**RSLCompilerCmd/**     RSLCompilerCmd application project and sources
**RSLCompilerLib/**     RSL compiler library
*CMakeLists.txt*        Root CMake file to create the build files
*license.txt*           License file
*make_build.bat*        Script that creates the ``../build`` directory and its contents
*make_distrib.bat*      Script that creates the ``../Distrib`` directory and its contents
*make_install.bat*      Script that builds a .zip file for binary distribution (assumes EXEs have been manually compiled with VS)
*readme.txt*            Readme file in txt format (and reST)
*readme.html*           Readme file in HTML format
======================  =======================================================

The Distrib Directory
======================

A directory named ``Distrib`` is generated outside ``Source`` as part of the build process. This directory is automatically generated, and it can therefore be safely erased (unless one has any important changes in it !)

The ``Distrib`` directory contains both data coming from ``DistribSrc`` and executables generated when compiling with Visual Studio (see below).

When running, it's is advised to have the environment variable ``RIBTOOLS_DIR`` set to the ``Distrib`` directory.
For more informations on how to setup the environment variable, see the instructions at the [[User_Manual#General_Usage|General Usage]] section of the *User Manual*.

Compiling and running from VS
=============================

Unpack libtiff
--------------

**libtiff**'s source code is kept as the *tiff-3.9.2.zip* archive under the **externals/** folder.
Unzip the *tiff-3.9.2.zip* in its own location.

As the result of unzipping, the following directory structure is expected:

**ribtools**

* **externals/**
 * **tiff-3.9.2/**
    * **build/**
    * **libtiff/**
    * **COPYRIGHT**
    * **[...]**

Building the solution
---------------------

#. Double click on ***build.bat*** to run a script that will create the directory **Distrib/**
# .Open the directory **pc/** from the main directory<br/> 

# .Double click on ***RibTools.sln*** inside **pc/** to launch the main solution with Visual Studio 2008
# .From the Standard Toolbar select a *solution configuration* (**Debug** or **Release**) and a *platform configuration* (**Win32** or **x64**)

# .Build the solution by selecting the option from the **Build** menu (or press F7 or Ctrl+Alt+B, depending on the VS configuration)

Once the build is finished the **Distrib/** directory should contain 3 executables:

* **Extras/**
* **Install/**
* **TestScenes/**
* **TestsOutput/**
* **Resources/**
* *DelCompiledShaders.bat*
* *MakeTests.bat*
* *readme.html*
* *RibRender.exe*
* *RibRender.pdb*
* *RibRenderServer.exe*
* *RibRenderServer.pdb*
* *RibRenderToy.exe*
* *RibRenderToy.pdb*
* *RSLCompilerCmd.exe*
* *RSLCompilerCmd.pdb*

Note that for every *.exe* file there is a corresponding *.pdb* file.

PDB files hold debug data necessary to display symbols when debugging with Visual Studio's debugger.

A little test
-------------
* From the **Distrib/** directory, launch ***RibRenderToy.exe***
* Right-click inside the RibRenderToy window and select a test scene from the pop-up menu (*Airplane.rib* is a safe bet)

[[Image:develop-select-airplane-scene.png|200px]]

* After a little while the scene should appear in all it's beauty (!)

[[Image:Airplane.png|200px]]

Running from Visual Studio
--------------------------

In order to run form visual studio, a one time setup is necessary.

Set the choosen application as the StartUp project
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Right-click on an executable project (e.g. RibRenderToy), and choose **Set as StartUp Project** from the pop-up menu.

[[Image:Develop-set-as-startup-prj.png|200px]]

Setup the working directory
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Right-click on the application project (e.g. RibRenderToy) and select **Properties **from the pop-up menu
* Find the **Working Directory **setting in the **Properties **dialog by selecting *Configuration Properties*** **-> *Debugging*
* Set the **Working Directory** to **..\..\Distrib** 

[[Image:Develop-working-dir.png|200px]]

**Note:** Set this for all the configurations (*Release* and *Debug*) and all platforms (*Win32* and *x64*).
A common mistake is to set up the Working Directory only for the current configuration and forget about the other configurations.

Run (!!!)
~~~~~~~~~

Run the application with or without the debugger by pressing F5, Ctrl+F5 (or whatever is your configuration 8)
