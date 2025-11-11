                                  




cmake_minimum_required(VERSION 3.19.2)
project(eHaz-Game-Engine)

 set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)



set(CMAKE_CXX_STANDARD 20)
 set(CMAKE_EXPORT_COMPILE_COMMANDS ON)




 if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -fdiagnostics-color=always
    )
endif()




option(BUILD_SHARED_LIBS "Build shared instead of static libraries" ON)
 add_compile_definitions(PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
# Add subdirectories
add_subdirectory(Engine)
add_subdirectory(SandBox)

