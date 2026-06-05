cmake_minimum_required(VERSION 3.5)

project(SPSCRingBuffer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SPSCRingBuffer main.cpp
    spsc_ring_buffer.h)

include(GNUInstallDirs)
install(TARGETS SPSCRingBuffer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
