mkdir build
cd build
cmake ..  -G "Visual Studio 16 2019" -A x64  -Dprotobuf_BUILD_TESTS=OFF -DUSE_JEMALLOC=OFF -DCMAKE_BUILD_TYPE=$1
cmake --build .  --config $1
cp bin/core/$1/remote_imgui_client.exe ../bin/
cp lib/core/$1/*.dll ../bin/
cp ../../server-depends/lib/*.dll ../bin/
