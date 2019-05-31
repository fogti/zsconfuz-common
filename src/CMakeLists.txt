cmake_minimum_required(VERSION 3.6)

add_executable(zsconfuz-filter filter.c)
install(TARGETS zsconfuz-filter DESTINATION "${INSTALL_BIN_DIR}")

add_library(zsconfuz-common SHARED cmdqueue.cxx write.cxx)
set_target_properties (zsconfuz-common PROPERTIES VERSION "0.0.0" SOVERSION 0)
target_compile_options(zsconfuz-common PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)

install(TARGETS zsconfuz-common DESTINATION "${INSTALL_LIB_DIR}" EXPORT "${CMAKE_PREFIX}Targets")
install(EXPORT "${CMAKE_PREFIX}Targets" DESTINATION "${INSTALL_CMAKE_DIR}" NAMESPACE "${CMAKE_PREFIX}::" COMPONENT dev)
install(FILES cmdqueue.hpp dflout.h write.hpp
  DESTINATION "${INSTALL_INCLUDE_DIR}/zs/confuz-common")

foreach(i cqmerge cqsanitize push)
  set(j "zsconfuz-${i}")
  add_executable("${j}" "${i}.cxx")
  target_link_libraries("${j}" zsconfuz-common)
  install(TARGETS "${j}" DESTINATION "${INSTALL_BIN_DIR}")
endforeach()
