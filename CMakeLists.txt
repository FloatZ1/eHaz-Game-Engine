                                  




cmake_minimum_required(VERSION 3.19.2)
project(eHaz-Game-Engine)

 set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)



set(CMAKE_CXX_STANDARD 20)
 set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

 add_compile_options(-w)
  add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/Debug
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/Debug
    COMMENT "Building project in Debug mode"
)

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/Release
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/Release
    COMMENT "Building project in Release mode"
)

add_custom_target(dist
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=RelWithDebInfo -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/Dist
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/Dist
    COMMENT "Building project for distribution (RelWithDebInfo)"
)


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

