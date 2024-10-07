@echo off
mkdir build_msvc_2022 > nul 2> nul
cmake -G "Visual Studio 17 2022" -A x64 -Bbuild_msvc_2022 . -DCMAKE_BUILD_TYPE=Release -DCF_LUA_LUAJIT=OFF --compile-no-warning-as-error
