cmake_minimum_required(VERSION 3.17)
project(Raytracer)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(Raytracer main.cpp Geom/Ray.cpp Geom/Ray.h Geom/Sphere.cpp Geom/Sphere.h Hittable_List.cpp Hittable_List.h consts.h Camera.cpp Camera.h Materials/Material.h Lambertian.h Geom/AABB.cpp Geom/AABB.h BVH/BVH.h ImageTexture.cpp SceneGraph/Texture.h Geom/Rect.cpp Geom/Rect.h SceneGraph/Node.h SceneGraph/Transform.h SceneGraph/Geometry.h Utils/Consts.h Utils/Structs.h Loader/OBJLoader.h Loader/Loader.h Utils/FileRenderOutput.h Utils/ColorBufferFormat.h Utils/ColorBufferFormatPPM.h Utils/Tile.h Geom/Intersection.h Materials/VNMaterial.h SceneGraph/Mesh.h Materials/ImageTexture.h Loader/ImageTextureLoader.h Materials/TXTMaterial.h Materials/Texture.h)

add_executable(Test test.cpp Libs/stb_image.h Utils/Buffer.h Utils/RenderOutput.h Utils/FileRenderOutput.h Utils/ColorBufferFormat.h Utils/ColorBufferFormatPPM.h Renderer.h Utils/Tile.h Geom/Intersections.h Geom/Intersection.h SceneGraph/Hittable.h Materials/Material.h Materials/VNMaterial.h SceneGraph/Mesh.h Materials/ImageTexture.h Loader/ImageTextureLoader.h Materials/TXTMaterial.h Materials/Texture.h)

target_link_libraries(Raytracer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Test ${CMAKE_THREAD_LIBS_INIT})