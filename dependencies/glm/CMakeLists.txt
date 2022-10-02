message(STATUS "Fetching glm...")

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG		   0.9.9.8
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
	FetchContent_Populate(glm)
	set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()

FetchContent_MakeAvailable(glm)

add_library(dependency_glm INTERFACE)
target_include_directories(dependency_glm SYSTEM INTERFACE "${glm_SOURCE_DIR}")
target_link_libraries(dependency_glm INTERFACE glm)