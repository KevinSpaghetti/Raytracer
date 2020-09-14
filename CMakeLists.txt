cmake_minimum_required(VERSION 3.17)
project(Raytracer)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(Test
        test.cpp
        Libs/stb_image.h
        Utils/Buffer.h
        Output/RenderOutput.h
        Output/FileRenderOutput.h
        Output/ColorBufferFormat.h
        Output/ColorBufferFormatPPM.h
        Renderer.h
        Utils/Tile.h
        Geom/Intersections.h
        Geom/Intersection.h
        SceneGraph/Hittable.h
        Materials/Material.h
        Materials/VNMaterial.h
        Mesh/Mesh.h
        Textures/ImageTexture.h
        Loader/ImageTextureLoader.h
        Materials/TXTMaterial.h
        Textures/Texture.h
        Utils/Random.h
        Mesh/TriangleMesh.h
        Mesh/SphereMesh.h
        SceneGraph/IntersectTestable.h
        BVH/Boxable.h
        BVH/AABB.h
        BVH/CanQueryForHits.h
        Textures/CheckerTexture.h
        Textures/SolidColorTexture.h
        Geom/ObjectIntersection.h Utils/Triangle.h
        AccelerationMethods/RegularGrid.h
        AccelerationMethods/ExhaustiveSearch.h
        AccelerationMethods/AccelerationStructure.h
        Mesh/TriangleMeshData.h
        BVH/BoundingBox.h
        Mesh/PlaneMesh.h
        Materials/Lambertian.h RenderInfo.h Materials/Metal.h)

target_link_libraries(Test ${CMAKE_THREAD_LIBS_INIT})