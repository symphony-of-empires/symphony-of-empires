set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

get_target_property(INCLUDE_DIRS OpenGL::GL INTERFACE_INCLUDE_DIRECTORIES)

add_library(dependency_OpenGL INTERFACE)
target_include_directories(dependency_OpenGL SYSTEM INTERFACE ${INCLUDE_DIRS})
target_link_libraries(dependency_OpenGL INTERFACE OpenGL::GL)