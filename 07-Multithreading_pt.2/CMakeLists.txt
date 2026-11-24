cmake_minimum_required(VERSION 3.16)
project(cups LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CUPS_CORE_SOURCES
  src/network/address.cpp
  src/network/file_descriptor.cpp
  src/network/poller.cpp
  src/network/socket.cpp
  src/network/timestamp.cpp
  src/cups/state/IsPrintingState.cpp
  src/cups/state/OffState.cpp
  src/cups/state/OnState.cpp
  src/cups/state/SuspendState.cpp
  src/cups/User.cpp
  src/cups/Spooler.cpp
  src/cups/Printer.cpp
)

add_library(cups_core STATIC ${CUPS_CORE_SOURCES})
target_include_directories(cups_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(cups_core PRIVATE -Werror
  $<$<CONFIG:Release>:-Os>
  $<$<CONFIG:Debug>:-g;-pg>
)
target_compile_definitions(cups_core PUBLIC $<$<CONFIG:Debug>:__DEBUG>)
target_link_libraries(cups_core PUBLIC pthread m)

foreach(app server client printer)
  add_executable(${app} src/${app}.cpp)
  target_compile_options(${app} PRIVATE -Werror
    $<$<CONFIG:Release>:-Os>
    $<$<CONFIG:Debug>:-g;-pg>
  )
  target_link_libraries(${app} PRIVATE cups_core)
  set_target_properties(${app} PROPERTIES
    OUTPUT_NAME $<IF:$<CONFIG:Debug>,${app}d,${app}>
  )
endforeach()
