cmake_minimum_required(VERSION 3.5)

SET(TARGET_NAME service_win)

project(service_win LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${TARGET_NAME} main.cpp)

configure_file(manifest.man ${CMAKE_BINARY_DIR} COPYONLY)
configure_file(register_log.cmd ${CMAKE_BINARY_DIR} COPYONLY)
configure_file(unregister_log.cmd ${CMAKE_BINARY_DIR} COPYONLY)
configure_file(register_service.cmd ${CMAKE_BINARY_DIR} COPYONLY)
configure_file(unregister_service.cmd ${CMAKE_BINARY_DIR} COPYONLY)

add_custom_target(
    compile_manifest
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/compile_manifest.cmd
)
add_dependencies(${TARGET_NAME} compile_manifest)

set( LOG_MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/manifest.res )
set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "${LOG_MANIFEST} /MANIFESTUAC:\"level='asInvoker' uiAccess='false'\"")

target_link_libraries(${TARGET_NAME}
        advapi32.lib
)
