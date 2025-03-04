# Microfluidic Nucleation Software

## To run the software on windows
- Install the precompiled opencv binaries to `C:\\opencv`
- use Visual Studio as build tool on Windows
- add `C:\opencv\build\x64\vc16\bin` to the `PATH` enviroment variable
- create a `.cache` directory, in the directory where the executabe resides (necessary for the OPENCV OpenCL DNN backend)
- add `-DCMAKE_PREFIX_PATH=C:\OpenCV\build` to the CMake options in CLion (general setting not configurations)
- 