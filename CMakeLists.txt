cmake_minimum_required(VERSION 3.6)
project(zsconfuz-common)

set(CMAKE_PREFIX "ZSconfuzCommon")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_LIB_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR "${INSTALL_LIB_DIR}/cmake/${CMAKE_PREFIX}" CACHE PATH "Installation directory for CMake module files")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-plt -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-plt")

add_subdirectory(cmake)
add_subdirectory(src)
