cmake_minimum_required(VERSION 3.17)
project(Raytracer)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(Test
        test.cpp
        Libs/stb_image.h
        Utils/Buffer.h
        Utils/RenderOutput.h
        Utils/FileRenderOutput.h
        Utils/ColorBufferFormat.h
        Utils/ColorBufferFormatPPM.h
        Renderer.h
        Utils/Tile.h
        Geom/Intersections.h
        Geom/Intersection.h
        SceneGraph/Hittable.h
        Materials/Material.h
        Materials/VNMaterial.h
        Mesh/Mesh.h
        Materials/ImageTexture.h
        Loader/ImageTextureLoader.h
        Materials/TXTMaterial.h
        Materials/Texture.h
        Utils/Random.h
        Mesh/TriangleMesh.h
        Mesh/SphereMesh.h
        SceneGraph/IntersectTestable.h
        BVH/Boxable.h
        BVH/AABB.h
        BVH/CanQueryForHits.h
        Materials/CheckerTexture.h
        Materials/SolidColorTexture.h
        Geom/ObjectIntersection.h Utils/Triangle.h AccelerationMethods/RegularGrid.h AccelerationMethods/ExhaustiveSearch.h AccelerationMethods/AccelerationStructure.h Mesh/TriangleMeshData.h BVH/BoundingBox.h)

target_link_libraries(Test ${CMAKE_THREAD_LIBS_INIT})