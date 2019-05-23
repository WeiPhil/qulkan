cmake_minimum_required(VERSION 3.10)
project (Qulkan)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()



# TODO : should make conditional if people only care about OpenGL
# find vulkan package (make sure environment variables are set) VK_ICD_FILENAMES and VULKAN_SDK for
find_package(Vulkan)

# find OpenMP
find_package(OpenMP)

# find libpng
find_package(PNG REQUIRED)
message("include dirs: " ${PNG_INCLUDE_DIRS})

# disable GLFW docs, examples and tests
# see http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# add glm without test suite
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_EXPERIMENTAL ON CACHE BOOL "" FORCE)
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

# apply a patch to setup imconfig.h 
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/imconfig.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/ext/imgui)
# Adding cpp files per directory
aux_source_directory(${SRC_DIR} SRC_FILES)
aux_source_directory(${SRC_DIR}/framework SRC_FILES_FRAMEWORK)
aux_source_directory(${SRC_DIR}/imgui SRC_FILES_IMGUI)
aux_source_directory(${SRC_DIR}/utils SRC_FILES_UTILS)
aux_source_directory(${SRC_DIR}/opengl SRC_FILES_OPENGL)
aux_source_directory(${SRC_DIR}/opengl/examples/basic    SRC_FILES_OPENGL_EXAMPLES_BASIC)
aux_source_directory(${SRC_DIR}/opengl/examples/lighting SRC_FILES_OPENGL_EXAMPLES_LIGHTING)
aux_source_directory(${SRC_DIR}/qulkan SRC_FILES_QULKAN)



if(Vulkan_FOUND)
  add_executable(Qulkan 
      ${SRC_FILES} 
      ${SRC_FILES_UTILS}
      ${SRC_FILES_FRAMEWORK}
      ${SRC_FILES_IMGUI}
      ${SRC_FILES_OPENGL}
      ${SRC_FILES_OPENGL_EXAMPLES_BASIC}
      ${SRC_FILES_OPENGL_EXAMPLES_LIGHTING}
      ${SRC_FILES_QULKAN}
  )
  target_link_libraries(Qulkan glm glfw gl3w imgui Vulkan::Vulkan OpenMP::OpenMP_CXX PNG::PNG)
else()
  message(STATUS "Vulkan library not found, Qulkan will be built without Vulkan support.")
  # manually remove vulkan dependencies
  list(REMOVE_ITEM SRC_FILES_IMGUI ${SRC_DIR}/imgui/imgui_impl_vulkan.cpp)

  add_executable(Qulkan 
      ${SRC_FILES} 
      ${SRC_FILES_UTILS}
      ${SRC_FILES_FRAMEWORK}
      ${SRC_FILES_IMGUI}
      ${SRC_FILES_OPENGL}
      ${SRC_FILES_OPENGL_EXAMPLES_BASIC}
      ${SRC_FILES_OPENGL_EXAMPLES_LIGHTING}
      ${SRC_FILES_QULKAN}
  )
  target_link_libraries(Qulkan glm glfw gl3w imgui OpenMP::OpenMP_CXX PNG::PNG)

endif()