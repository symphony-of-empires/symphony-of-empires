cmake_minimum_required(VERSION 3.5.1)

project(SymphonyOfEmpires)

option(BUILD_ENGINE "Enable building UnifiedRender" ON)
option(BUILD_GAME "Enable building SymphonyOfEmpires" ON)

# Backend selection
IF(UR_BACKEND_OPENGL)
	message("Using OpenGL backend")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUR_BACKEND_OPENGL=1")
ELSE()
	IF(UR_BACKEND_D3D9)
		message("Using DirectX 9 SDK")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUR_BACKEND_D3D9=1")
	ELSE()
		
	ENDIF()
ENDIF()

# Build stuff
if(BUILD_ENGINE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/unified_render)
endif()

if(BUILD_GAME)
    add_subdirectory(${CMAKE_SOURCE_DIR}/game)
endif()