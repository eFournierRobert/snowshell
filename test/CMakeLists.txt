find_package(Criterion QUIET)
if(NOT Criterion_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(CRITERION REQUIRED criterion)
endif()

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

foreach(src ${TEST_SOURCES})
  get_filename_component(test_name "${src}" NAME_WE)
  add_executable(${test_name}
      "${src}"
      ${SRC_DIR}/dir.c)
  if(TARGET Criterion::Criterion)
    target_link_libraries(${test_name} PRIVATE Criterion::Criterion)
  else()
    target_include_directories(${test_name} PRIVATE ${CRITERION_INCLUDE_DIRS})
    target_link_libraries(${test_name} PRIVATE ${CRITERION_LINK_LIBRARIES})
  endif()
  target_include_directories(${test_name} PRIVATE ${SRC_DIR})
  target_compile_definitions(${test_name} PRIVATE TESTING)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()