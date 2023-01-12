include(FetchContent)

add_subdirectory(OpenGL)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(GLEW)
add_subdirectory(zlib)
set(BUILD_SHARED_LIBS ON)

add_subdirectory(assimp)
add_subdirectory(SDL2)
add_subdirectory(SDL2_ttf)
add_subdirectory(glm)
add_subdirectory(LUA)
add_subdirectory(eventpp)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(tbb)
set(BUILD_SHARED_LIBS ON)
