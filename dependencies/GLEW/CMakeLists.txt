message(STATUS "Fetching GLEW...")

FetchContent_Declare(
    GLEW
    URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
    URL_MD5 970535b75b1b69ebd018a0fa05af63d1 # 2.2.0
)

FetchContent_MakeAvailable(GLEW)

add_library(GLEW "${glew_SOURCE_DIR}/src/glew.c")
target_include_directories(GLEW PUBLIC "${glew_SOURCE_DIR}/include")
target_link_libraries(GLEW PUBLIC dependency_OpenGL)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(GLEW PRIVATE GLEW_BUILD)
else()
    target_compile_definitions(GLEW PUBLIC GLEW_STATIC)
endif()

add_library(dependency_GLEW INTERFACE)
target_include_directories(dependency_GLEW SYSTEM INTERFACE $<TARGET_PROPERTY:GLEW,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(dependency_GLEW INTERFACE GLEW)