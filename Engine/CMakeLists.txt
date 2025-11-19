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

# ────────────────────────────────────────────────
# Build eHazEngine itself
# ────────────────────────────────────────────────
file(GLOB_RECURSE EHAZENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp"
  
)

add_library(eHazEngine SHARED ${EHAZENGINE_SOURCES})

target_include_directories(eHazEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/UI/ImGui
)

# ────────────────────────────────────────────────
# Link against EnvHazGraphics
# ────────────────────────────────────────────────
target_link_libraries(eHazEngine
    PUBLIC EnvHazGraphics
)
