# How To Build

Just use CMake. You might have to install CMake into your development environment.

XScreenSaverWin supports build by GCC, Clang, and MSVC.

After build, double clicking batch file `pack.bat` will deploy the files.

The installer source is `installer.iss`,
that's an Inno Setup script. Please run Inno Setup after deployment.

You cannot install all the screensavers at once.
You may not put so many .scr files in one folder directly.
It fails .scr installation.
There's limitation of Windows in the numbers of installables at once.
