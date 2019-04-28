cmake_minimum_required(VERSION 3.10)
project (Qulkan)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# find vulkan package (make sure environment variables are set) VK_ICD_FILENAMES and VULKAN_SDK
find_package(vulkan REQUIRED)

# find OpenMP
find_package(OpenMP REQUIRED)

# disable GLFW docs, examples and tests
# see http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# set path to dependencies
add_subdirectory(ext/glfw)
include_directories(ext/glfw/include)
include_directories(ext/imgui)

# compile gl3w and find_package
execute_process(
COMMAND cmake .
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext/gl3w
)
execute_process(
COMMAND cmake --build .
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext/gl3w
)
find_package(gl3w REQUIRED)

# imgui implementation (compiled as a library)
aux_source_directory(ext/imgui IMGUI_SRC_FILES)
set(IMGUI_INCLUDE_DIR imgui ext/imgui)
add_library(imgui STATIC ${IMGUI_SRC_FILES})

# set includes of project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SRC_DIR src)
include_directories(${SRC_DIR} ${IMGUI_INCLUDE_DIR})
aux_source_directory(${SRC_DIR} SRC_FILES)
add_executable(Qulkan ${SRC_FILES})
target_link_libraries(Qulkan glfw gl3w imgui Vulkan::Vulkan OpenMP::OpenMP_CXX)