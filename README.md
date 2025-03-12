# Microfluidic Nucleation Software

## General
- Set the environment variable `OPENCV_OCL4DNN_CONFIG_PATH=.cache`

## To run the software on windows
- Install the precompiled opencv binaries to `C:\\opencv`
- use Visual Studio as build tool on Windows
- add `C:\opencv\build\x64\vc16\bin` to the `PATH` enviroment variable
- create a `.cache` directory, in the directory where the executabe resides (necessary for the OPENCV OpenCL DNN backend)
- add `-DCMAKE_PREFIX_PATH=C:\OpenCV\build` to the CMake options in CLion (general setting not configurations)

## Linux
- Install the `mesa-opencl-icd` package

## Converting the dynamic input shape model to fixed 
> At the moment this doesn't work, as it messes up the neural network shape.

OpenCV currently doesn't support ONNX models with a variable batch size (dynamic axes), therefore the model is converted to a fixed size.
The input size is determined by the `--dim-value` parameter.
```
pip install onnxruntime onnx
python -m onnxruntime.tools.make_dynamic_shape_fixed --dim_param batch --dim_value 24 dynamic_input_model.onnx  fixed_24_model.onnx
```