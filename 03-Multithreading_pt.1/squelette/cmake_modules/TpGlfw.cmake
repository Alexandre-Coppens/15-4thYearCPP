# Inclusion commune pour un exercice TP2 (C++20 + pthread + GLFW/OpenGL/epoxy/glm).
# Avant include : définir TP_SOURCES.

cmake_minimum_required(VERSION 3.10)
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

if(NOT DEFINED TP_SOURCES)
  message(FATAL_ERROR "TP_SOURCES doit être défini avant d'inclure TpGlfw.cmake")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(FATAL_ERROR "Ce TP se compile avec gcc (CXX=${CMAKE_CXX_COMPILER_ID}).")
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(TP_BIN tp)
else()
  file(RELATIVE_PATH TP_BIN "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
  string(REPLACE "/" "_" TP_BIN "${TP_BIN}")
endif()

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(EPOXY REQUIRED epoxy)
find_path(GLM_INCLUDE_DIR glm/glm.hpp REQUIRED)

add_executable(${TP_BIN} ${TP_SOURCES})
set_target_properties(${TP_BIN} PROPERTIES
  OUTPUT_NAME tp
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(${TP_BIN} PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  ${GLM_INCLUDE_DIR}
  ${EPOXY_INCLUDE_DIRS})
target_link_libraries(${TP_BIN} PRIVATE
  Threads::Threads
  OpenGL::GL
  glfw
  ${EPOXY_LIBRARIES})
target_compile_definitions(${TP_BIN} PRIVATE
  SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
target_compile_options(${TP_BIN} PRIVATE -Wall -Wextra -Wpedantic)
