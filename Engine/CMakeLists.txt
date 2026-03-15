#cmake_minimum_required(VERSION 3.19.2)
#project(eHazEngine LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)




# (optional) compiler warnings, settings, etc.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#option(BUILD_SHARED_LIBS "Build shared instead of static libraries" ON)
add_compile_definitions(PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}" JPH_DEBUG_RENDERER)

# ────────────────────────────────────────────────
# Add dependency as a subdirectory
# ────────────────────────────────────────────────
add_subdirectory(dependencies/EnvHaz/EnvHaz-Graphics/lib/)


 include(FetchContent)
# set(CMAKE_C_COMPILER /usr/bin/clang CACHE STRING "" FORCE)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE STRING "" FORCE)

#if(UNIX)


#------------------Lua---------------------
 FetchContent_Declare(
	sol2
	GIT_REPOSITORY https://github.com/ThePhD/sol2.git
	GIT_TAG main
)


 FetchContent_MakeAvailable(sol2)
# Lua library



 # ─────────────── Sol2 ───────────────




#-------------end lua---------------------











FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.12.2
)


set(CPP_RTTI_ENABLED ON CACHE BOOL "" FORCE)
set(JPH_DEBUG_RENDERER ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
        GIT_TAG v5.5.0
		SOURCE_SUBDIR "Build"
)


FetchContent_MakeAvailable(JoltPhysics)
FetchContent_MakeAvailable(EnTT)
set(CROSS_PLATFORM_DETERMINISTIC ON)
set(CPP_RTTI_ENABLED ON)
 target_compile_definitions(Jolt PUBLIC JPH_DEBUG_RENDERER


)

#elseif(WIN32)

 #find_package(EnTT REQUIRED)   
#endif()
 find_package(Boost REQUIRED)

SET(LUA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua")


add_library(
    lua STATIC
    ${LUA_DIR}/lapi.c
    ${LUA_DIR}/lauxlib.c
    ${LUA_DIR}/lbaselib.c
    ${LUA_DIR}/lcode.c
    ${LUA_DIR}/lcorolib.c
    ${LUA_DIR}/lctype.c
    ${LUA_DIR}/ldblib.c
    ${LUA_DIR}/ldebug.c
    ${LUA_DIR}/ldo.c
    ${LUA_DIR}/ldump.c
    ${LUA_DIR}/lfunc.c
    ${LUA_DIR}/lgc.c
    ${LUA_DIR}/linit.c
    ${LUA_DIR}/liolib.c
    ${LUA_DIR}/llex.c
    ${LUA_DIR}/lmathlib.c
    ${LUA_DIR}/lmem.c
    ${LUA_DIR}/loadlib.c
    ${LUA_DIR}/lobject.c
    ${LUA_DIR}/lopcodes.c
    ${LUA_DIR}/loslib.c
    ${LUA_DIR}/lparser.c
    ${LUA_DIR}/lstate.c
    ${LUA_DIR}/lstring.c
    ${LUA_DIR}/lstrlib.c
    ${LUA_DIR}/ltable.c
    ${LUA_DIR}/ltablib.c
    ${LUA_DIR}/ltm.c
    ${LUA_DIR}/lundump.c
    ${LUA_DIR}/lutf8lib.c
    ${LUA_DIR}/lvm.c
    ${LUA_DIR}/lzio.c
)

 target_include_directories(lua
    PUBLIC
        ${LUA_DIR}
)
# ────────────────────────────────────────────────
# Build eHazEngine itself
# ────────────────────────────────────────────────
file(GLOB_RECURSE EHAZENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp"
  
)

add_library(eHazEngine STATIC ${EHAZENGINE_SOURCES})
 target_compile_definitions(eHazEngine
    PUBLIC
        SOL_ALL_SAFETIES_ON=1
        SOL_USE_STD_OPTIONAL=1
        
)
target_include_directories(eHazEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/UI/ImGui
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Scene/
         ${CMAKE_CURRENT_SOURCE_DIR}/include/UI/ 
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Utility/
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Scripting
         ${CMAKE_CURRENT_SOURCE_DIR}/dependencies
         ${Boost_INCLUDE_DIRS}
         ${LUA_DIR}
)

# ────────────────────────────────────────────────
# Link against EnvHazGraphics
# ────────────────────────────────────────────────
target_link_libraries(eHazEngine
    PUBLIC EnvHazGraphics
    PUBLIC EnTT::EnTT
    PUBLIC Jolt
    PUBLIC lua
    PUBLIC sol2::sol2

)



