mkdir ..\lib
mkdir ..\bin

protobuf()
{
cd protobuf/
mkdir build_cmake
cd build_cmake
cmake ../cmake  -G "Visual Studio 16 2019" -A x64  -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_EXAMPLES=OFF -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release
copy Release\protoc.exe ..\..\..\bin\ /Y
copy Release\libprotobuf* ..\..\..\lib\ /Y
cd ../../
}

libevent()
{
cd libevent
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -DEVENT__DISABLE_OPENSSL=ON -DEVENT__DISABLE_BENCHMARK=ON -DEVENT__DISABLE_TESTS=ON -DEVENT__DISABLE_REGRESS=ON -DEVENT__DISABLE_SAMPLES=ON -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL
cmake --build . --config Release
copy lib\Release\*.lib ..\..\..\lib\ /Y
copy bin\Release\*.dll ..\..\..\lib\ /Y
cd ../..
}

fmt()
{
cd fmt
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -DFMT_TEST=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
copy Release\fmt.lib ..\..\..\lib\ /Y
cd ../..
}

cryptopp()
{
cd cryptopp
mkdir cmake_build
cd cmake_build
cmake ..  -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DDISABLE_ASM=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL
cmake --build . --config Release
copy Release\cryptopp* ..\..\..\lib\ /Y
cd ../..
}


cryptopp()
{
cd lz4/build/VS2017
msbuild.exe -target:liblz4 -property:Configuration=Release -property:Platform=x64

copy bin\x64_Release\*.lib ..\..\..\..\lib\ /Y
cd ../../..
}


