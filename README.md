# Sim Rt

Simple Ray tracing framework ***Sim Rt*** is based on the mini-book series of ***Peter Shirley*** that can be found on his [github](https://github.com/petershirley). It's a hobby project to learn about the basics of CPU Ray tracing and modern C++17.

## Results
Cornell Box                                    | Material test                    | Environment map
:---------------------------------------------:|:--------------------------------:|:-------------------------------------:
![Cornell Box](github/cornell.png)             | ![Material](github/material.png) | ![Environment map](github/envmap.png)
Meshs and instancing                           | Texture test                     | Debug rays
![Mesh Instancing](github/mesh-instancing.png) | ![Texture](github/texture.png)   | ![Debug rays](github/debug.png)


## Topics

This sections covers algorithms and techniques that might be implemented in the future.

- **BRDF** - to model the reflectance properties of a material
- **PBR** - uses the BRDF, found [here](https://learnopengl.com/PBR/Theory)
- **Photon Mapping** - to get those nice caustics
- **Acceleration Structures** - to step away from a trivial implementation
  - **BVH**
- **Monte Carlo Integration** - found in the [pbr-book](http://www.pbr-book.org/3ed-2018/Monte_Carlo_Integration.html)

## Scene Format

The main application can be used as a command line tool. It can be called with

```.\sim-rt.exe <SCENE_PATH> <OUTPUT_IMAGE_PATH>?```

while the path of the output image path is optional. If it's not specified it defaults to:

```.\sim-rt.exe <SCENE_PATH> .\unnamed.png```

### Structure

The file consists of 5 parts namely ***TRACER, CAMERA, MATERIALS, OBJECTS*** and ***SCENE***. The parts should be specified in the file in this order to avoid unexpected errors as the scene is constructed on the fly and might depend on previously defined parts. In the following all 5 parts are described in detail.

#### TRACER

Specifies the type of tracer, the image resolution of the image plane and thus the resolution of the output image, the number of samples per pixel and the trace depth per ray.

```
TRACER
    TYPE       raytracer
    RESOLUTION 1080 720
    SAMPLES    100
    DEPTH      100
```

So far there are three different types of tracers ***raycaster, raytracer*** and ***debugtracer***. The debugtracer renders the intersections between the rays and the scene and displays those intersections as spheres, while rendering the scene from a different angle than the camera.

The resolution keyword has to be followed by two positive integer numbers  bigger than zero. The first number is the width of the output image and the second number is the height of the output image.

The number of samples has to be followed by one positive integer number bigger than zero. The number of samples specifies the number of rays emitted per pixel. The resulting colors per ray are averaged to get the final pixel color.

The depth keyword is followed by a positive integer number bigger than zero. It specifies the trace depth i.e. the number of indirections.

#### CAMERA



## License

The MIT License (MIT)

Copyright (c) 2019 Adrian Derstroff

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.