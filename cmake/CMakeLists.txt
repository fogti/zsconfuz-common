cmake_minimum_required(VERSION 3.7)

foreach(i "Find${CMAKE_PREFIX}" "${CMAKE_PREFIX}Config")
  set(j "${PROJECT_BINARY_DIR}/${i}.cmake")
  configure_file("${i}.cmake.in" "${j}" @ONLY)
  install(FILES "${j}" DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
endforeach(i)
