message(STATUS "Fetching SDL2 Mixer...")

set(SDL2MIXER_VENDORED ON)
set(SDL2MIXER_BUILD_SHARED_LIBS OFF)
set(SDL2MIXER_INSTALL OFF)
set(SDL2MIXER_SAMPLES OFF)
FetchContent_Declare(sdl2_mixer 
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_mixer" 
    GIT_TAG 75f318100d220e236c4fcc5a3d0c9ba33d064f3f # 2.20.1
)
FetchContent_MakeAvailable(sdl2_mixer)
add_library(dependency_SDL2_mixer INTERFACE)
target_include_directories(dependency_SDL2_mixer SYSTEM INTERFACE "${sdl2_mixer_SOURCE_DIR}" "${sdl2_mixer_BINARY_DIR}")
target_link_libraries(dependency_SDL2_mixer INTERFACE SDL2_mixer)
target_link_libraries(dependency_SDL2_mixer INTERFACE dependency_freetype)
