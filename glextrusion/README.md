

GLE - The GL Extrusion Library
==============================
This is an update to the very old GLE library, still available
[here](https://www.linas.org/gle/).

This update is a snapshot of the final version 3.1.0 of the codebase,
and includes work from Dave Richards, providing a MSWindows-friendly
C++ wrapper. Dave's work is in the [cgle-c++](cgle-c++) directory.

GLE
---

GLE is a library package of C functions that draw
extruded surfaces, including surfaces of revolution,
sweeps, tubes, polycones, polycylinders and helicoids.
Generically, the extruded surface is specified with a
2D polyline that is extruded along a 3D path.  A local
coordinate system allows for additional flexibility in 
the primitives drawn.  Extrusions may be texture mapped
in a variety of ways.  The GLE library generates 3D 
triangle coordinates, lighting normal vectors and 
texture coordinates as output. GLE uses the GL or 
OpenGL(R) API's to perform the actual rendering.
The demos use GLUT and require GLUT to be installed.

Obtaining OpenGL
----------------
OpenGL is available on most UNIX(R) workstations,
as well as OS/2(R) and Windows NT.  Contact your
workstation vendor for more information; the URL
http://www.opengl.org/ points to a variety of 
information, including a list of OpenGL vendors. 
GLE also works with Mesa, a public-domain 
OpenGL-like API. Mesa can be found at 
http://www.mesa3d.org/

Obtaining GLUT
--------------
The demos require that the GLUT windowing and 
utility library be installed.  GLUT can be obtained
at http://reality.sgi.com/mjk_asd/glut3/glut3.html

On Debian
---------
On modern Linux systems, skip the above, and instead, just say:
```
sudo apt-get install libgle3-dev
```
This will give you everything in this git repo, precompiled.
If you want to compile from source, you will need this:
```
apt-get install libgl1-mesa-dev libxmu-dev libxi-dev freeglut3-dev
```

Compiling
---------
GLE uses the standard GNU automake/autoconf build process,
and thus should be portable to essentially all computing 
platforms.  At the command line simply type 'configure'
to set things up for your CPU & operating system.  Then run
'make' to compile. Finally, cd to the examples directory, 
and run the script 'rundemo' to launch each of the demos 
in order (from most basic, to advanced).  Use the left 
mouse button to move, the middle mouse button to access
the pop-down menu. 

If 'configure' fails, its probably because some library
or another is missing.  Look at the file 'config.log'
to see what actually went wrong.  In particular, pay
attention to the short program at the end: trying to 
compile it by hand will give you a good idea on why
it failed.

configure flags:
--enable-lenient-tess
	If you have a tesselator that is happy with anything,
	including degenerate points, colinear segments, etc.
	then define this. Otherwise, don't specify this flag.
        Setting this flag provides a minor performance improvement.
	
	I beleive that the stock SGI tesselator is "lenient",
	despite explicit disclaimers in the documentation.
	(circa 1995).  
	
        Early versions of the MesaGL tesselator are not at all 
        forgiving of degenerate points.  This resulted in frequent 
        crashes and/or hangs.  (circa 1997-2000). Recent versions
        (as of 2001) seem to work fine.  If you have an old version
        of MesaGL, do not set  <tt>--enable-lenient-tess</tt>

--disable-auto-texture
	Disable texture mapping code.  Disabling texture 
	mapping may provide a very minor performance improvement.

--enable-irisgl
        Compile for old IrisGL/GL-3.2 API.  This used to work, but
        hasn't been tested in a long time.

--enable-debug
	Will compile sources so printf routines will be called instead
        of OpenGL routines.  Warning: this will generate a *lot* of
        output!


More information about building, as well as this package, 
can be found in the directory "public_html".


RedHat RPM's
------------
RedHat RPM's can be built using the gle.spec file.


Python, SWIG
------------
Python bindings for gle can be found in the /swig directory.
Be sure to read the readme.

Compiling for Windows NT
------------------------
To compile with Visual C++, just do the following:

cd src
cl -c -DWIN32  *.c
lib -out:libgle.lib *.obj

Alternately, there are a set of Microsoft Visual C Studio Project
files in the directory ms-visual-c that should do the same thing.


Running
-------
Some of the example programs will hang or crash when run on
older versions of MesaGL/Linux.  This is due to bugs in the 
MesaGL tesellator.  Newer versions should work fine.
