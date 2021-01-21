# Setting a development environment on Windows

## Dependencies to install

*IMPORTANT*: Reboot after you're done installing them !

### Visual Studio Community 2019

https://visualstudio.microsoft.com/fr/vs/community/

### CMake

https://cmake.org/download/

Use the .msi installer and add it to the PATH for all users.

### Qt 5.15 (not 6)

https://www.qt.io/download

Note: you just need to install the basic stuff for MSVC 2019 64-bit, not the source packages, Android, UWP, WinRT, iOS stuff or all the extended components which will take up a ton of disk space for nothing.

### git

https://gitforwindows.org/

### Boost

https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.7z

Just extract the folder somewhere, e.g. c:\dev\boost_1_75_0

### Python 3

https://www.python.org/downloads/release/python-391/

Put it in the path when the installer asks for it.

Once Python 3 is installed, open a shell with it, and install the dependencies for building the documentation:

```bash
$ pip install sphinx quark-sphinx-theme
```

## Cloning the repo

From git bash:

```bash
$ git clone --recursive -j16 https://github.com/scrime-u-bordeaux/segment-editor
```

## Building

### Option A: editing / building with Qt Creator

* Open Qt Creator
* Go inside the settings for a quick sanity check:
    * Select the "Kits" settings on the left list with the icons.
    * Go to the "Compilers" tab, check that MSVC has been detected.
    * Go to the "Qt Versions" tab, check that the Qt version has been detected.
      If it hasn't, you must add it by selecting the qmake.exe (from the folder you have installed Qt it should be something like `5.15/msvc2019_64/bin/qmake.exe`). Apply and restart the IDE.
    * Go to the "Kits" tab, check that there is a Kit setup with MSVC 64-bit and the Qt version and select it.
    * At the bottom line, select the "Change" button next to "CMake Configuration".
    * In the panel that opens, add the line `BOOST_ROOT=c:/dev/boost_1_75_0` (or the folder where you extracted it - it should contain a `boost` subfolder). Note: the slashes in the path are `/`, not `\` !

* Open the CMakeLists.txt at the root of the SEGMent repo, press Next when Qt Creator prompts you.
* Check that the build works by pressing the big "Play" button: SEGMent should launch after the build ends.

### Option B: editing / building with Visual Studio

* Open a Visual Studio command-line: in the start menu look for "visual studio x64 native tools" or something like that.
* Create a build directory where you want the build to live.
* Inside that build directory, run the following command:

```bash
$ cmake ^
  c:/path/to/segment-editor ^
  -T host=x64 -A x64 -G "Visual Studio 16 2019" ^
  -DCMAKE_PREFIX_PATH=c:/path/to/Qt/5.15/msvc2019_64/ ^
  -DBOOST_ROOT=c:/path/to/boost_1_75_0
```

Note: the slashes in the path are `/`, not `\` !

* This should create a segment-editor.sln that can be opened with Visual Studio for editing and building the code.

* Note: never edit the .sln files themselves as every modification will be lost. Only ever edit the CMake files for adding source files, etc.
