cmake_minimum_required(VERSION 3.11)

project(self-organizing-tree-models)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_definitions(-Wall)
add_definitions(-Wextra)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-DDEBUGGING)
endif()

include_directories(glad/include)

add_executable(self-organizing-tree-models
               glad/src/glad.c
               src/Application.cpp
               src/BoundingBox.cpp
               src/BoundingBox.hpp
               src/Random.cpp
               src/Random.hpp
               src/Point.cpp
               src/Point.hpp
               src/Tree.cpp
               src/Tree.hpp
               src/Environment.cpp
               src/Environment.hpp
               src/OpenGlWindow.cpp
               src/OpenGlWindow.hpp
               src/Metamer.cpp
               src/Metamer.hpp
               src/Vertex.cpp
               src/Vertex.hpp
               src/Image.cpp
               src/Image.hpp
               src/MarkerSet.cpp
               src/MarkerSet.hpp
               src/Color.cpp
               src/Color.hpp
               src/Range.cpp
               src/Range.hpp
               src/Types.cpp
               src/Types.hpp
               src/Text.cpp
               src/Text.hpp
               src/Vector.cpp
               src/Vector.hpp
               src/SpaceAnalysis.cpp
               src/SpaceAnalysis.hpp
               src/PointAverage.cpp
               src/PointAverage.hpp
               src/MarkerSetRanges.cpp
               src/MarkerSetRanges.hpp
               src/Marker.cpp
               src/Marker.hpp
               src/UserAction.cpp
               src/UserAction.hpp)

find_package(glm REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(self-organizing-tree-models ${GLFW_LIBRARIES})
target_link_libraries(self-organizing-tree-models ${GLFW_STATIC_LIBRARIES})
target_link_libraries(self-organizing-tree-models ${OpenCV_LIBS})
