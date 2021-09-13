## A simple raytracer
This repository showcases a implementation of a (somewhat complete) raytracing engine using the backward raytracing technique. 
It starts with the description of a scene and generates a picture in the PPM format.

It's based on the famous [peter shirley raytracing tutorials](https://raytracing.github.io/books/RayTracingInOneWeekend.html), expanding them
by adding a lot of structure to load and handle resources efficiently.
Some of the things added are:
- Triangle meshes
- Other acceleration structures
- PBR workflow
- Different types of samplers
- Integrators (as explained in [this pixar paper](https://graphics.pixar.com/library/RendermanTog2018/paper.pdf))

## Pictures generated

Pictures generated with a debug integrator that samples the normals and one that samples the colors.

![NormalDebugIntegrator](./images/NormalDebugIntegrator.png)
![BackwardIntegrator](./images/BackwardIntegrator.png)

The typical raytracing noise can be seen in the second picture that has been rendered with a low number of samples. 

## Info

This project was built during my third year at university and has been used as a graduation project.
It's built using idiomatic C++17 and STL, trying to take advantage of all new features to improve code quality and efficiency.

The 2 major challenges in this project are:
- The interplay of many expensive (to copy and build) data structures (scene graphs, meshes, acceleration structures, image buffers) needed to carry out the raytracing algorithm.
- Minimizing the time that it takes to trace the path of a single ray.  

The correct usage of C++ was therefore essential to guarantee that every operation gets carried out as 
fast as possible (avoiding memory allocation, reuse the memory, etc) and with minimum overhead 
(moving instead of copying, perfect forwarding, etc) 