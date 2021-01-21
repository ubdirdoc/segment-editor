# Setting a development environment on macOS

## Dependencies to install

### Xcode

Through the AppStore

### Homebrew

https://brew.sh

### Python and pip

Python is already available on macOS. pip is however not.

```bash
$ curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
$ pip install sphinx quark-sphinx-theme
```

### Homebrew dependencies

From a terminal:

```bash
$ brew install cmake ninja boost python
```

### Qt

Two possibilities:
1. Through Homebrew: `brew install qt`. The simplest, but will only work on Macs that are as recent as the machine the build is being done on (e.g. if you are on 10.14 users will need at least 10.14).

2. Through a manually installed Qt version: https://github.com/ubdirdoc/segment-editor/releases/download/sdk/qt-5.9.7-macos.tar.gz

Use the second one to be able to provide binaries that will work starting from macOS 10.10.


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
    * Go to the "Compilers" tab, check that Xcode (clang) has been detected.
    * Go to the "Qt Versions" tab, check that the Qt version has been detected.
      If it hasn't, you must add it by selecting the qmake binary, in `/usr/local/Cellar/qt/bin/qmake` if installed through homebrew. Apply and restart the IDE.
    * Go to the "Kits" tab, check that there is a Kit setup with Xcode and the Qt version and select it.

* Open the CMakeLists.txt at the root of the SEGMent repo, press Next when Qt Creator prompts you.
* Check that the build works by pressing the big "Play" button: SEGMent should launch after the build ends.

### Option B: editing / building with Xcode

* Open a shell.
* Create a build directory where you want the build to live.
* Inside that build directory, run the following command:

```bash
$ cmake \
  /path/to/segment-editor \
  -GXcode \
  -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15/clang_64
```

(Note: if using Homebrew, Qt will be located in /usr/local/Cellar/qt)

* This should create a segment-editor.xcodeproj that can be opened with Xcode for editing and building the code.


* Note: never edit the Xcode files themselves as every modification will be lost. Only ever edit the CMake files for adding source files, etc.
