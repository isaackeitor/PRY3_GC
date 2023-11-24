cmake_minimum_required(VERSION 3.26.4)
project(Josuelintrivilin VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

find_package(SDL2 REQUIRED)

target_include_directories(${PROJECT_NAME}
  PRIVATE ${PROJECT_SOURCE_DIR}/include
  PUBLIC ${SDL2_INCLUDE_DIRS}
  PUBLIC /opt/homebrew/include/SDL2
  PUBLIC /opt/homebrew/Cellar/glm/0.9.9.8/include
)

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARIES}
  /opt/homebrew/lib/libSDL2_image.dylib  # Enlazar libSDL2_image.dylib
)
