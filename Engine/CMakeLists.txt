#cmake_minimum_required(VERSION 3.19.2)
#project(eHazEngine LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)




# (optional) compiler warnings, settings, etc.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#option(BUILD_SHARED_LIBS "Build shared instead of static libraries" ON)
add_compile_definitions(PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

# ────────────────────────────────────────────────
# Add dependency as a subdirectory
# ────────────────────────────────────────────────
add_subdirectory(dependencies/EnvHaz/EnvHaz-Graphics/lib/)


 include(FetchContent)
# set(CMAKE_C_COMPILER /usr/bin/clang CACHE STRING "" FORCE)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE STRING "" FORCE)

#if(UNIX)

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.12.2
)




FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
        GIT_TAG "v5.5.0"
		SOURCE_SUBDIR "Build"
)
FetchContent_MakeAvailable(JoltPhysics)
FetchContent_MakeAvailable(EnTT)



#elseif(WIN32)

 #find_package(EnTT REQUIRED)   
#endif()
 find_package(Boost REQUIRED)




# ────────────────────────────────────────────────
# Build eHazEngine itself
# ────────────────────────────────────────────────
file(GLOB_RECURSE EHAZENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp"
  
)

add_library(eHazEngine STATIC ${EHAZENGINE_SOURCES})

target_include_directories(eHazEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/UI/ImGui
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Scene/
         ${CMAKE_CURRENT_SOURCE_DIR}/include/UI/ 
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Utility/
          ${Boost_INCLUDE_DIRS}
)

# ────────────────────────────────────────────────
# Link against EnvHazGraphics
# ────────────────────────────────────────────────
target_link_libraries(eHazEngine
    PUBLIC EnvHazGraphics
    PUBLIC EnTT::EnTT
    PUBLIC Jolt
)



