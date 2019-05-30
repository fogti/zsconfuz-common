cmake_minimum_required(VERSION 3.6)

add_executable(zsconfuz-filter filter.c)

add_library(zsconfuz-common SHARED cmdqueue.cxx write.cxx)
set_target_properties (zsconfuz-common PROPERTIES VERSION "0.0.0" SOVERSION 0)
target_compile_options(zsconfuz-common PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)

add_executable(zsconfuz-cqsanitize cqsanitize.cxx)
target_link_libraries(zsconfuz-cqsanitize zsconfuz-common)

add_executable(zsconfuz-push push.cxx)
target_link_libraries(zsconfuz-push zsconfuz-common)

install(TARGETS zsconfuz-common DESTINATION "${INSTALL_LIB_DIR}" EXPORT "${CMAKE_PREFIX}Targets")
install(EXPORT "${CMAKE_PREFIX}Targets" DESTINATION "${INSTALL_CMAKE_DIR}" NAMESPACE "${CMAKE_PREFIX}::" COMPONENT dev)

install(TARGETS zsconfuz-filter zsconfuz-cqsanitize zsconfuz-push
  DESTINATION "${INSTALL_BIN_DIR}")
install(FILES cmdqueue.hpp dflout.h write.hpp
  DESTINATION "${INSTALL_INCLUDE_DIR}/zs/confuz-common")
