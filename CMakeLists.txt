cmake_minimum_required(VERSION 3.0)

if(CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES Release Debug DebugOpenGLCalls)
   set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} CACHE STRING "Append user-defined configuration to list of configurations to make it usable in Visual Studio" FORCE)
endif()

project(OGLFramework_uulm)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

get_filename_component(FW_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR} REALPATH)
set(FW_RESOURCE_DIR "resources" CACHE STRING "Resources directory in project structure.")
set(FW_RESOURCE_BASE_PATH ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/${FW_RESOURCE_DIR})

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX /EHsc")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-function -Wno-unused-parameter -Wextra -Wpedantic -std=c++14")
endif()
LIST(APPEND CUDA_NVCC_FLAGS -gencode arch=${FWLIB_CUDA_ARCH},code=${FWLIB_CUDA_CODE})

set(COMPILE_DEFINITIONS_DEBUGOPENGLCALLS ${COMPILE_DEFINITIONS_DEBUG})
set(CMAKE_CXX_FLAGS_DEBUGOPENGLCALLS ${CMAKE_CXX_FLAGS_DEBUG})
set(CMAKE_C_FLAGS_DEBUGOPENGLCALLS ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_EXE_LINKER_FLAGS_DEBUGOPENGLCALLS ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGOPENGLCALLS ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})

add_subdirectory(OGLFrameworkLib)
configure_file("${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/patternsConfig.xml" "patternsConfig.xml")


file(GLOB_RECURSE RES_FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/${FW_RESOURCE_DIR}/*.*)
file(GLOB TOP_FILES
    ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/*.h
	${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/*.cpp
	${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/*.cu)
file(GLOB_RECURSE TMP_FILES
    ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/*.h
    ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/*.cpp
	${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/*.cu)
LIST(REMOVE_ITEM TMP_FILES ${RES_FILES})
LIST(REMOVE_ITEM TMP_FILES ${TOP_FILES})

foreach(f ${TMP_FILES})
    file(RELATIVE_PATH SRCGR ${CMAKE_SOURCE_DIR}/${PROJECT_NAME} ${f})
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()
source_group(" " FILES ${TOP_FILES})


foreach(f ${RES_FILES})
    file(RELATIVE_PATH SRCGR ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/${FW_RESOURCE_DIR} ${f})
	set(SRCGR "Resources/${SRCGR}")
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()

file(GLOB_RECURSE RES_OBJ_FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/${FW_RESOURCE_DIR}/*.obj)
foreach(f ${RES_OBJ_FILES})
	set_source_files_properties(${f} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()


file(GLOB PROJECT_CONFIGS CMakeLists.txt Readme.md .gitattributes .gitignore .gitmodules)

# add_definitions(-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
include_directories(${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/)
cuda_add_executable(${PROJECT_NAME} WIN32 ${TMP_FILES} ${TOP_FILES} ${RES_FILES} ${PROJECT_CONFIGS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC FreeImage glew32 opengl32 OGLFrameworkLib)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:DebugOpenGLCalls>:_OGL_DEBUG_MSGS> _CRT_SECURE_NO_WARNINGS)

file(GLOB CUDA_DLLS ${CUDA_TOOLKIT_ROOT_DIR}/bin/cudart*.dll)
foreach(f ${CUDA_DLLS})
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${f} ${CMAKE_BINARY_DIR})
endforeach()
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/${FWLIB_LIBNAME}/$<CONFIGURATION>/${FWLIB_LIBNAME}.dll" ${CMAKE_BINARY_DIR})
