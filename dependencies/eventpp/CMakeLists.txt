message(STATUS "Fetching glm...")

FetchContent_Declare(
	eventpp
	GIT_REPOSITORY https://github.com/wqking/eventpp
	GIT_TAG		   v0.1.2
)

# FetchContent_GetProperties(glm)
# if(NOT glm_POPULATED)
# 	FetchContent_Populate(glm)
# 	set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
# 	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
# endif()

FetchContent_MakeAvailable(eventpp)

add_library(dependency_eventpp INTERFACE)
target_include_directories(dependency_eventpp SYSTEM INTERFACE "${eventpp_SOURCE_DIR}")
target_link_libraries(dependency_eventpp INTERFACE eventpp)