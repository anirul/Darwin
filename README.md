# Darwin

A very simple [gRPC](https://grpc.io) based client server application.

## Building

You will need the followings:

- [Git](https://git-scm.com/)
- [CMake](https://cmake.org/)
- A C++ compiler (you can use Visual Studio)

To build the server part and then the client you need to follow the following procedures:

1. You need to install [vcpkg](https://vcpkg.io/en/).
2. You need to build the project.

### Install VCPKG

Install [vcpkg](https://vcpkg.io/en/), from the [github](https://github.com/microsoft/vcpkg) page, if possible in the same subdirectory as the `darwin` project. The project was builded with the tag version: `2024.02.14`, but it should work with any version via minor changes.

```pwsh
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2024.02.14
./bootstrap-vcpkg.bat
```

Congratulation you now have VCPKG on you drive! Remember where you installed it as you will need this info in the next step.

### Use LFS

As I use various binary files (I m using texture and other non text files). You will need to install git LFS (Large File System).

```pwsh
git lfs pull
```

### Make the project

You now need to create a project for the compiler (and IDE) you want. You can use the `-G` option to select witch generator to use, it should select the default for your system if you don't use it.

```pwsh
git clone https://github.com/anirul/darwin.git
cd Darwin
git submodule update --init --recursive
mkdir Build
cd Build
cmake .. -DCMAKE_TOOLCHAIN_FILE="[path to VCPKG]/scripts/buildsystems/vcpkg.cmake"
```

Now either open the .sln file or use cmake to build the project.

```pwsh
cmake --build .
```
