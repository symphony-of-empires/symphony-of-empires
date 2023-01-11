message(STATUS "Fetching Assimp...")

set(ASSIMP_INSTALL OFF)
set(ASSIMP_IGNORE_GIT_HASH ON)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ZLIB OFF)
set(ASSIMP_BUILD_MINIZIP OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(BUILD_LIB_TYPE STATIC)
set(ENABLE_BOOST_WORKAROUND ON)



FetchContent_Declare(assimp
    GIT_REPOSITORY "https://github.com/assimp/assimp"
    GIT_TAG 9519a62dd20799c5493c638d1ef5a6f484e5faf1
)


FetchContent_MakeAvailable(assimp)

add_library(dependency_assimp INTERFACE)

target_include_directories(dependency_assimp SYSTEM INTERFACE "${assimp_BINARY_DIR}/include" "${assimp_SOURCE_DIR}/include")
# target_include_directories(dependency_assimp INTERFACE assimp)
