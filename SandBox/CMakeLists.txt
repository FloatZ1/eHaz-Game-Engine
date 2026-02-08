                                
# app/CMakeLists.txt

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(SandBox ${APP_SOURCES})

target_include_directories(SandBox PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(SandBox PRIVATE eHazEngine)

# ------------------------------
# Add a custom run target that uses NVIDIA GPU
# ------------------------------

target_compile_definitions(eHazEngine PUBLIC
    eRESOURCES_PATH="${CMAKE_SOURCE_DIR}/SandBox/assets/"
    eEXTERNAL_LIB_PATH="${CMAKE_SOURCE_DIR}/dependencies/"
)  

if(UNIX)
add_custom_target(run
    COMMAND env __NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia $<TARGET_FILE:SandBox>
    DEPENDS SandBox
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running SandBox on NVIDIA GPU..."
)  

endif()

if(WIN32)

add_custom_command(TARGET SandBox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:EnvHazGraphics>
        $<TARGET_FILE_DIR:SandBox>
)


endif()




