cmake_minimum_required(VERSION 4.0)
project(snowshell-docs NONE)
include(GNUInstallDirs)

find_program(PANDOC_EXECUTABLE pandoc) 
if(NOT PANDOC_EXECUTABLE)
  message(FATAL_ERROR "pandoc not found")
endif()

file(GLOB MAN_MD_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/man/*.md"
)

message(STATUS "Found man MD sources: ${MAN_MD_SOURCES}")
if(NOT MAN_MD_SOURCES)
  message(WARNING "No Markdown manpages in ${CMAKE_CURRENT_SOURCE_DIR}/man")
endif()

set(MAN_OUT_DIR "${CMAKE_BINARY_DIR}/man")
set(GENERATED_MANPAGES)

foreach(src ${MAN_MD_SOURCES})
  get_filename_component(fname "${src}" NAME)      # snowshell.1.md
  get_filename_component(name_we "${src}" NAME_WE) # snowshell.1
  string(REGEX REPLACE ".*\\.([0-9]+)\\.md$" "\\1" section "${fname}")

  if(NOT section MATCHES "^[0-9]+$")
    message(FATAL_ERROR "Cannot determine man section for ${fname} (expected name.<section>.md)")
  endif()

  set(out "${MAN_OUT_DIR}/${name_we}")             # build/man/snowshell.1

  add_custom_command(
    OUTPUT "${out}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${MAN_OUT_DIR}"
    COMMAND "${PANDOC_EXECUTABLE}" -s -t man "${src}" -o "${out}"
    DEPENDS "${src}"
    BYPRODUCTS "${out}"
    COMMENT "pandoc ${src} -> ${out}"
    VERBATIM
  )

  list(APPEND GENERATED_MANPAGES "${out}")
  install(FILES "${out}" DESTINATION "${CMAKE_INSTALL_MANDIR}/man${section}")
endforeach()

add_custom_target(manpages ALL DEPENDS ${GENERATED_MANPAGES})
