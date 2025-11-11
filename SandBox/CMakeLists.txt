                                
# app/CMakeLists.txt

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(SandBox ${APP_SOURCES})

target_include_directories(SandBox PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(SandBox PRIVATE EnvHazGraphics)

# ------------------------------
# Add a custom run target that uses NVIDIA GPU
# ------------------------------
add_custom_target(run
    COMMAND env __NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia $<TARGET_FILE:SandBox>
    DEPENDS SandBox
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running SandBox on NVIDIA GPU..."
)  
