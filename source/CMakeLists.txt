set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/bios.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bios.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dma.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dma.h
    ${CMAKE_CURRENT_SOURCE_DIR}/instruction_set.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/instruction_set.h
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_map.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ram.h
    ${CMAKE_CURRENT_SOURCE_DIR}/registers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/registers.h
    ${CMAKE_CURRENT_SOURCE_DIR}/type_helpers.h
)

add_library(${TARGET_NAME_LIB} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${SOURCE_FILES}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX source FILES ${SOURCE_FILES})

set_target_properties(${TARGET_NAME_LIB} PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
endif()

add_executable(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${TARGET_NAME}
    ${TARGET_NAME_LIB}
)

set_target_properties(${TARGET_NAME} PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
