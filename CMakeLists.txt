cmake_minimum_required(VERSION 4.0)
project(snowshell C)
include(GNUInstallDirs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.c)
add_executable(snowshell ${SRC_FILES})

add_subdirectory(doc)

target_include_directories(snowshell PRIVATE src)
target_compile_options(snowshell PRIVATE -Wall -Wextra -Werror -g)

add_custom_target(run 
  COMMAND snowshell
  DEPENDS snowshell
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Builds and run snowshell")

configure_file(
  "${CMAKE_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake)

install(TARGETS snowshell RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})