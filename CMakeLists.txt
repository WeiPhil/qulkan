cmake_minimum_required(VERSION 3.10)
project (Qulkan)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# find vulkan package (make sure environment variables are set) VK_ICD_FILENAMES and VULKAN_SDK for
find_package(Vulkan REQUIRED)

# find OpenMP
find_package(OpenMP)

# disable GLFW docs, examples and tests
# see http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# add glm without test suite
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(ext/glm)

# set path to dependencies
add_subdirectory(ext/glfw)
include_directories(ext/glfw/include)
include_directories(ext/imgui)

# compile gl3w and add generated file to sources
execute_process(
COMMAND cmake .
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext/gl3w
)
execute_process(
COMMAND cmake --build .
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext/gl3w
)
add_subdirectory(ext/gl3w)
set(GL3W_SRC ext/gl3w/src/gl3w.c)

# imgui implementation (compiled as a library)
aux_source_directory(ext/imgui IMGUI_SRC_FILES)
set(IMGUI_INCLUDE_DIR imgui ext/imgui)
add_library(imgui STATIC ${IMGUI_SRC_FILES})

# set includes of project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SRC_DIR src)
include_directories(${SRC_DIR} ${IMGUI_INCLUDE_DIR})

# Adding cpp files per directory
aux_source_directory(${SRC_DIR} SRC_FILES)
aux_source_directory(${SRC_DIR}/framework SRC_FILES_FRAMEWORK)
aux_source_directory(${SRC_DIR}/imgui SRC_FILES_IMGUI)
aux_source_directory(${SRC_DIR}/utils SRC_FILES_UTILS)


add_executable(Qulkan 
    ${SRC_FILES} 
    ${SRC_FILES_UTILS}
    ${SRC_FILES_FRAMEWORK}
    ${SRC_FILES_IMGUI}
)
target_link_libraries(Qulkan glm glfw gl3w imgui Vulkan::Vulkan OpenMP::OpenMP_CXX)