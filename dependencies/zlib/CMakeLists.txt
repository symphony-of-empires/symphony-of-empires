message(STATUS "Fetching zlib...")

FetchContent_Declare(zlib
    GIT_REPOSITORY "https://github.com/madler/zlib"
)

FetchContent_MakeAvailable(zlib)

add_library(dependency_zlib INTERFACE)

target_include_directories(dependency_zlib SYSTEM INTERFACE "${zlib_SOURCE_DIR}")
target_link_libraries(dependency_zlib INTERFACE zlib)
