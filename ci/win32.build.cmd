mkdir build
cd build

set PATH=%PATH%;c:\ossia-sdk\llvm\bin
cmake -GNinja %BUILD_SOURCESDIRECTORY% ^
  -DCMAKE_C_COMPILER=c:/ossia-sdk/llvm/bin/clang.exe ^
  -DCMAKE_CXX_COMPILER=c:/ossia-sdk/llvm/bin/clang++.exe ^
  -DSEGMENT_RELEASE=1 ^
  -DOSSIA_SDK=c:\ossia-sdk ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_PREFIX_PATH="c:/ossia-sdk/qt5-static;c:/ossia-sdk/llvm-libs;c:/ossia-sdk/SDL2;c:/ossia-sdk" ^
  -DCMAKE_UNITY_BUILD=1

cmake --build .
cmake --build . --target package
