message(STATUS "Fetching LUA...")

set(LUA_BUILD_BINARY OFF)
set(LUA_BUILD_COMPILER OFF)
# IF(E3D_BUILD_STATIC OR CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
	set(LUA_ENABLE_SHARED OFF)
	set(LUA_SUPPORT_DL OFF)
# ELSE()
	# set(LUA_ENABLE_SHARED ON)
	# set(LUA_SUPPORT_DL ON)
# ENDIF()
FetchContent_Declare(lua 
    GIT_REPOSITORY "https://github.com/walterschell/Lua"
)

FetchContent_MakeAvailable(lua)

add_library(dependency_lua INTERFACE)

target_include_directories(dependency_lua SYSTEM INTERFACE "${lua_SOURCE_DIR}/lua-5.4.4/include")
target_link_libraries(dependency_lua INTERFACE lua_static)