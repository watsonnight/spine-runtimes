
@REM change path to emsdk path
cmake -DCMAKE_TOOLCHAIN_FILE=D:/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -B buildwasm -DCMAKE_BUILD_TYPE=Release -G "Ninja" ../CMakeLists.txt

cmake --build buildwasm

cd buildwasm
del spine-cpp-lite.a
ren libspine-cpp-lite.a spine-cpp-lite.a
cd ..