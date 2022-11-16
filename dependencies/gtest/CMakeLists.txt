message(STATUS "Fetching gtest...")

FetchContent_Declare(
	gtest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG		   release-1.12.1
)
FetchContent_GetProperties(gtest)
if(NOT gtest_POPULATED)
	FetchContent_Populate(gtest)
	set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
	add_subdirectory(${gtest_SOURCE_DIR} ${gtest_BINARY_DIR})
endif()
FetchContent_MakeAvailable(gtest)

add_library(dependency_gtest INTERFACE)
target_include_directories(dependency_gtest SYSTEM INTERFACE "${gtest_SOURCE_DIR}")
target_link_libraries(dependency_gtest INTERFACE gtest)
