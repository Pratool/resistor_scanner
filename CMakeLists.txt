cmake_minimum_required(VERSION 3.8.2)

project("Resistor Scanner")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(OpenCV 4.1 REQUIRED)
find_package(spdlog REQUIRED)

add_executable(resistor_scanner
  resistor_scanner.cc
)

target_link_libraries(resistor_scanner
  ${OpenCV_LIBS}
  spdlog::spdlog
)

install(TARGETS resistor_scanner
  DESTINATION bin
)
