cmake_minimum_required(VERSION 3.5.1)

project(SymphonyOfEmpires)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS} -pthread")

IF(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dwindows")
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dunix")
ENDIF()

# Workaround for clang (see https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/SDL2")
link_directories(/usr/local/lib)

# Helps github workflows to not die
IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
ENDIF()

include_directories("${PROJECT_SOURCE_DIR}")

# C++ source files
file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp" "${PROJECT_SOURCE_DIR}/client/*.cpp" "${PROJECT_SOURCE_DIR}/client/interface/*.cpp" "${PROJECT_SOURCE_DIR}/client/ui/*.cpp" "${PROJECT_SOURCE_DIR}/server/*.cpp" "${PROJECT_SOURCE_DIR}/server/ai/*.cpp")
file(GLOB RENDER_SOURCES "${PROJECT_SOURCE_DIR}/unified_render/*.cpp")
add_executable(SymphonyOfEmpires ${MAIN_SOURCES} ${RENDER_SOURCES})

# Precompiled headers
file(GLOB MAIN_HEADERS "${PROJECT_SOURCE_DIR}/*.hpp" "${PROJECT_SOURCE_DIR}/client/*.hpp" "${PROJECT_SOURCE_DIR}/client/interface/*.hpp" "${PROJECT_SOURCE_DIR}/client/ui/*.hpp" "${PROJECT_SOURCE_DIR}/server/*.hpp" "${PROJECT_SOURCE_DIR}/server/ai/*.hpp")
file(GLOB RENDER_HEADERS "${PROJECT_SOURCE_DIR}/unified_render/*.hpp")
target_precompile_headers(SymphonyOfEmpires PUBLIC ${MAIN_HEADERS} ${RENDER_HEADERS})

IF(WIN32)
	target_link_libraries(SymphonyOfEmpires PUBLIC glew32 opengl32 glu32 ws2_32 winpthread intl wsock32)
ELSE()
	target_link_libraries(SymphonyOfEmpires PUBLIC GL GLU GLEW SDL2main)
ENDIF()
target_link_libraries(SymphonyOfEmpires PUBLIC SDL2 SDL2_ttf m z tbb stdc++)

IF(NOT MINGW AND NOT MSYS)
	IF(lua54)
		target_link_libraries(SymphonyOfEmpires PUBLIC lua5.4)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA54")
		message("Making with Lua 5.4")
	ELSE()
		target_link_libraries(SymphonyOfEmpires PUBLIC lua5.3)
		message("Making with Lua 5.3")
	ENDIF()
# msys is simple - just lua, no lua5.4 or lua5.3
ELSE()
	target_link_libraries(SymphonyOfEmpires PUBLIC lua)
	message("Making with unknown lua")
ENDIF()
