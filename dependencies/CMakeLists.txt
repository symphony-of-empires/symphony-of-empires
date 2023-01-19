include(FetchContent)

add_subdirectory(OpenGL)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(GLEW)
add_subdirectory(zlib)
#add_subdirectory(assimp)
set(BUILD_SHARED_LIBS ON)

add_subdirectory(SDL2)
add_subdirectory(SDL2_ttf)
add_subdirectory(SDL2_mixer)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(glm)
add_subdirectory(LUA)
add_subdirectory(eventpp)

add_subdirectory(tbb)
set(BUILD_SHARED_LIBS ON)
